
#include "user-login.hpp"
#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace mtquiz_service {

namespace handlers {
namespace users {

namespace {

class UserLogin final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user-login";

  UserLogin(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("mtquiz-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {}

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendUserLogin(userver::components::ComponentList& component_list) {
  component_list.Append<UserLogin>();
}

}  // namespace users
}  // namespace handlers
}  // namespace mtquiz_service