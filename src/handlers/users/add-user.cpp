#include "add-user.hpp"

#include <fmt/format.h>

#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/formats/json/value.hpp>
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

const static std::regex passwordRegex(R"([\w@#$%\^&\?\*+=!()-]{1,50})");
const static std::regex usernameRegex(R"([\w]{1,200})", std::regex::icase);

class AddUser final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-user-add";

  AddUser(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("mtquiz-db-1")
                .GetCluster()) {}

  bool CheckUserInput(const std::string& username,
                      const std::string& password) const {
    return std::regex_match(password, passwordRegex) &&
           std::regex_match(username, usernameRegex);
  }

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    auto request_body =
        userver::formats::json::FromString(request.RequestBody());
    auto username = request_body["username"].As<std::optional<std::string>>();
    auto password = request_body["password"].As<std::optional<std::string>>();
    if (!username.has_value() || !password.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::BadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"Some parameter is missing",
                          security::ErrorTypes::kWrongAmountOfParameters}}
                          .ExtractValue());
    }
    if (!CheckUserInput(username.value(), password.value())) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::BadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{
              "password can contain alphabet characters and special "
              "characters. "
              "Username must only contain regular arphabet characters",
              security::ErrorTypes::kWrongParameterFormat}}
                          .ExtractValue());
    }
    repositories::UserRepository user_repo(pg_cluster_);
    auto user = user_repo.GetUserByUsername(username.value());
    if (user.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::BadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{
              "username is taken",
              security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    user = user_repo.CrateUser(username.value(), password.value());
    return user.value().id;
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendAddUser(userver::components::ComponentList& component_list) {
  component_list.Append<AddUser>();
}

}  // namespace users
}  // namespace handlers
}  // namespace mtquiz_service