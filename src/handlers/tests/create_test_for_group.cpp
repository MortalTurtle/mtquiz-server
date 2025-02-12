#include "create_test_for_group.hpp"

#include <fmt/format.h>

#include <optional>
#include <string>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
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
#include "repositories/tests_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace tests {

namespace {

class CreateTest final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-test-create";

  CreateTest(const userver::components::ComponentConfig& config,
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
    repositories::TestsRepository tests_repo(pg_cluster_);
    auto request_body =
        userver::formats::json::FromString(request.RequestBody());
    auto test_name = request_body["name"].As<std::optional<std::string>>();
    auto test_description =
        request_body["description"].As<std::optional<std::string>>();
    if (!test_name.has_value() || !test_description.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"one of arguments is missing",
                          security::ErrorTypes::kWrongAmountOfParameters}}
                          .ExtractValue());
    }
    try {
      auto test =
          tests_repo.CreateTest(test_name.value(), test_description.value(),
                                session->user_id, group_id);
      return test.id;
    } catch (userver::storages::postgres::ForeignKeyViolation ex) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"Group is not found",
                          security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendCreateTest(userver::components::ComponentList& component_list) {
  component_list.Append<CreateTest>();
}

}  // namespace tests
}  // namespace handlers
}  // namespace mtquiz_service