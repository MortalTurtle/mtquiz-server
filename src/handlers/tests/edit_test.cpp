#include "edit_test.hpp"

#include <fmt/format.h>

#include <optional>
#include <string>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/status_code.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/exceptions.hpp>
#include <userver/storages/postgres/io/traits.hpp>
#include <userver/utils/assert.hpp>
#include "lib/auth.hpp"
#include "models/Error.hpp"
#include "repositories/group_role_repo.hpp"
#include "repositories/tests_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace tests {

namespace {

class EditTest final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-test-edit";

  EditTest(const userver::components::ComponentConfig& config,
           const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("mtquiz-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    auto session = security::GetSessionInfo(pg_cluster_, request);
    if (!session.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kUnauthorized);
      return {};
    }
    auto group_id = request.GetPathArg("groupId");
    auto test_id = request.GetPathArg("id");
    repositories::GroupRoleRepository role_repo(pg_cluster_);
    repositories::TestsRepository tests_repo(pg_cluster_);
    auto test = tests_repo.GetTest(test_id);
    auto role_in_group =
        role_repo.GetUserRoleInGroup(session->user_id, group_id);
    if (!role_in_group.has_value() || role_in_group.value() != Roles::kOwner ||
        (test.has_value() && test->owner_id != session->user_id)) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kForbidden);
      return {};
    }
    if (!test.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kNotFound);
      return {};
    }
    auto response_body =
        userver::formats::json::FromString(request.RequestBody());
    auto description =
        response_body["description"].As<std::optional<std::string>>();
    auto name = response_body["name"].As<std::optional<std::string>>();
    auto min_score = response_body["minScoreToPass"].As<std::optional<int>>();
    if (min_score.has_value() && min_score < 0) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"min score to beat must be more or equals than zero",
                          security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    tests_repo.EditTest(test_id, name, description, min_score);
    return {};
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendEditTest(userver::components::ComponentList& component_list) {
  component_list.Append<EditTest>();
}

}  // namespace tests
}  // namespace handlers
}  // namespace mtquiz_service