#include "edit-user.hpp"

#include <fmt/format.h>

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
#include "repositories/user_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace users {
namespace {

// TODO make db to save old info
class EditUser final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user-edit";

  EditUser(const userver::components::ComponentConfig& config,
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
    repositories::UserRepository user_repo(pg_cluster_);
    auto user = user_repo.GetUserBySession(session.value());
    auto request_body =
        userver::formats::json::FromString(request.RequestBody());
    auto username = request_body["username"].As<std::optional<std::string>>();
    auto password = request_body["password"].As<std::optional<std::string>>();
    if (!username.has_value() && !password.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"No parameters passed",
                          security::ErrorTypes::kWrongAmountOfParameters}}
                          .ExtractValue());
    }
    bool has_user_with_new_username = false;
    if (username.has_value())
      has_user_with_new_username =
          user_repo.GetUserByUsername(username.value()).has_value();
    if (has_user_with_new_username) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"Username taken",
                          security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    user_repo.UpdateUserInfo(session->user_id, username, password);
    return {};
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendEditUser(userver::components::ComponentList& component_list) {
  component_list.Append<EditUser>();
}
}  // namespace users
}  // namespace handlers
}  // namespace mtquiz_service