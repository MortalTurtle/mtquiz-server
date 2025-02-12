#include "get_tests_for_group.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <optional>
#include <string>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/status_code.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include "lib/auth.hpp"
#include "models/Error.hpp"
#include "repositories/group_repo.hpp"
#include "repositories/tests_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace tests {

namespace {

class GetTests final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-get-tests";

  GetTests(const userver::components::ComponentConfig& config,
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
    repositories::GroupRepository group_repo(pg_cluster_);
    auto group = group_repo.GetGroup(group_id);
    if (!group.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"No group found",
                          security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    auto offset_str = request.GetArg("offset");
    auto limit_str = request.GetArg("limit");
    std::optional<int> offset =
        (offset_str == "" ? std::nullopt
                          : std::optional(std::stoi(offset_str)));
    std::optional<int> limit =
        (limit_str == "" ? std::nullopt : std::optional(std::stoi(limit_str)));
    repositories::TestsRepository tests_repo(pg_cluster_);
    auto tests = tests_repo.GetAllTestsForGroup(group_id, limit, offset);
    userver::formats::json::ValueBuilder item;
    std::for_each(tests.begin(), tests.end(), [&](auto test) {
      item.PushBack(userver::formats::json::ValueBuilder{test});
    });
    return ToString(item.ExtractValue());
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetTests(userver::components::ComponentList& component_list) {
  component_list.Append<GetTests>();
}

}  // namespace tests
}  // namespace handlers
}  // namespace mtquiz_service