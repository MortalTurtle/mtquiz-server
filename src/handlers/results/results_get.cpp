#include "results_get.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <limits>
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
#include <userver/utils/assert.hpp>
#include "lib/auth.hpp"
#include "models/Error.hpp"
#include "repositories/group_repo.hpp"
#include "repositories/group_role_repo.hpp"
#include "repositories/tests_repo.hpp"
#include "repositories/user_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace results {

namespace {

class GetResults final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-results-get";

  GetResults(const userver::components::ComponentConfig& config,
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
    bool are_for_test = request.HasArg("testId");
    repositories::TestsRepository test_repo(pg_cluster_);
    repositories::UserRepository user_repo(pg_cluster_);
    repositories::GroupRoleRepository role_repo(pg_cluster_);
    std::optional<Test> test = are_for_test
                                   ? test_repo.GetTest(request.GetArg("testId"))
                                   : std::nullopt;
    auto user = user_repo.GetUserById(session->user_id);
    if (!user->group_id.has_value() || (are_for_test && !test.has_value()) ||
        (are_for_test && test->group_id != user->group_id)) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{
              "user either not in group or if testId is passed there is not "
              "test or test group is not the same group as user is in",
              security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    repositories::GroupRepository group_repo(pg_cluster_);
    auto results = group_repo.GetUserTestResults(
        session->user_id, user->group_id.value(),
        request.HasArg("limit") ? stoi(request.GetArg("limit"))
                                : std::numeric_limits<int>::max(),
        request.HasArg("offset") ? stoi(request.GetArg("offset")) : 0,
        test.has_value() ? std::optional<std::string>(test->id) : std::nullopt);
    userver::formats::json::ValueBuilder item;
    std::for_each(results.begin(), results.end(),
                  [&](const TestResult& res) { item.PushBack(res); });
    return results.size() == 0 ? "[]" : ToString(item.ExtractValue());
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetResults(userver::components::ComponentList& component_list) {
  component_list.Append<GetResults>();
}

}  // namespace results
}  // namespace handlers
}  // namespace mtquiz_service