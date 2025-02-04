#include "get-user.hpp"

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/status_code.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include "models/Error.hpp"
#include "models/user.hpp"
#include "repositories/user_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace users {

namespace {

class GetUser final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user-get";

  GetUser(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("mtquiz-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override  {
    auto username = request.GetArg("username");
    auto id = request.GetArg("id");
    repositories::UserRepository repo(pg_cluster_);
    std::optional<User> user;
    if (username != "")
      user = repo.GetUserByUsername(username);
    if (id != "" && !user.has_value())
      user = repo.GetUserById(id);
    if (user.has_value()) {
      userver::formats::json::ValueBuilder item;
      item["id"] = user->id;
      item["username"] = user->username;
      if (user->group_id.has_value())
        item["groupId"] = user->group_id.value();
      return ToString(item.ExtractValue());
    }
    auto& response = request.GetHttpResponse();
    if (username == "" && id == "") {
      response.SetStatus(userver::http::kBadRequest);
      security::Error error{"No arguments provided", "Wrong number of arguments"};
      return ToString(userver::formats::json::ValueBuilder{error}.ExtractValue());
    }
    response.SetStatus(userver::http::kNotFound);
    return {};
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetUser(userver::components::ComponentList& component_list) {
  component_list.Append<GetUser>();
}

}  // namespace users
}  // namespace handlers
}  // namespace mtquiz_service