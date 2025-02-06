
#include "user-login.hpp"
#include <fmt/format.h>

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
#include "repositories/user_repo.hpp"

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
      userver::server::request::RequestContext&) const override {
    auto request_body =
        userver::formats::json::FromString(request.RequestBody());
    auto username = request_body["username"].As<std::optional<std::string>>();
    auto password = request_body["password"].As<std::optional<std::string>>();
    repositories::UserRepository user_repo(pg_cluster_);
    if (!username.has_value() || !password.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"Some parameter is missing",
                          security::ErrorTypes::kWrongAmountOfParameters}}
                          .ExtractValue());
    }
    auto user = user_repo.GetUserByUsername(username.value());
    if (!user.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kNotFound);
      return {};
    }
    auto session =
        security::TryLogin(pg_cluster_, username.value(), password.value());
    if (!session.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"Password is incorrect",
                          security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    return session->id;
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendUserLogin(userver::components::ComponentList& component_list) {
  component_list.Append<UserLogin>();
}

}  // namespace users
}  // namespace handlers
}  // namespace mtquiz_service