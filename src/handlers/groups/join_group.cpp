#include "join_group.hpp"

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
#include <userver/utils/assert.hpp>
#include "lib/auth.hpp"
#include "lib/not_found_exception.hpp"
#include "repositories/group_repo.hpp"
#include "repositories/group_role_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace groups {

namespace {

class JoinGroup final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-group-join";

  JoinGroup(const userver::components::ComponentConfig& config,
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
    auto group_id = request.GetPathArg("id");
    repositories::GroupRepository group_repo(pg_cluster_);
    auto group = group_repo.GetGroup(group_id);
    if (!group.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kNotFound);
      return {};
    }
    group_repo.JoinGroup(session->user_id, group_id);
    repositories::GroupRoleRepository role_repo(pg_cluster_);
    if (!role_repo.GetUserRoleInGroup(session->user_id, group_id).has_value())
      role_repo.AddRoleForUser(session->user_id, group_id, Roles::kParticipant);
    return {};
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendJoinGroup(userver::components::ComponentList& component_list) {
  component_list.Append<JoinGroup>();
}

}  // namespace groups
}  // namespace handlers
}  // namespace mtquiz_service