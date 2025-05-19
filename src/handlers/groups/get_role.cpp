#include "get_role.hpp"

#include <fmt/format.h>

#include <string>
#include <unordered_map>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/status_code.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include "lib/auth.hpp"
#include "models/group_role.hpp"
#include "repositories/group_role_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace groups {

namespace {

class GetGroupRole final : public userver::server::handlers::HttpHandlerBase {
  static inline std::unordered_map<Roles, std::string> role_to_str = {
      {Roles::kOwner, "kOwner"},
      {Roles::kContributor, "kContributor"},
      {Roles::kParticipant, "kParticipant"}};

 public:
  static constexpr std::string_view kName = "handler-group-get-role";

  GetGroupRole(const userver::components::ComponentConfig& config,
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
    auto id = request.GetPathArg("id");
    repositories::GroupRoleRepository role_repo(pg_cluster_);
    auto group_role = role_repo.GetUserRoleInGroup(session->user_id, id);
    if (!group_role.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kNotFound);
      return {};
    }
    return role_to_str[group_role.value()];
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetGroupRole(userver::components::ComponentList& component_list) {
  component_list.Append<GetGroupRole>();
}

}  // namespace groups
}  // namespace handlers
}  // namespace mtquiz_service