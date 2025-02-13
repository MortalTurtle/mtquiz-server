#include "question_create.hpp"

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
#include "models/Error.hpp"
#include "models/question_type.hpp"
#include "repositories/group_repo.hpp"
#include "repositories/group_role_repo.hpp"
#include "repositories/questions_repo.hpp"
#include "repositories/tests_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace questions {

namespace {

class CreateQuestion final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-questions-create";

  CreateQuestion(const userver::components::ComponentConfig& config,
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
    auto test_id = request.GetPathArg("testId");
    repositories::GroupRoleRepository role_repo(pg_cluster_);
    repositories::TestsRepository test_repo(pg_cluster_);
    auto test = test_repo.GetTest(test_id);
    if (!test.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"no such test found",
                          security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    auto role_in_group =
        role_repo.GetUserRoleInGroup(session->user_id, test->group_id);
    if (!role_in_group.has_value() || role_in_group == Roles::kParticipant ||
        (role_in_group == Roles::kContributor &&
         test->owner_id != session->user_id)) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kForbidden);
      return {};
    }
    auto request_body =
        userver::formats::json::FromString(request.RequestBody());
    auto question_text = request_body["text"].As<std::optional<std::string>>();
    auto question_type_str =
        request_body["type"].As<std::optional<std::string>>();
    if (!question_text.has_value() || !question_type_str.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"One or more parameters is missong",
                          security::ErrorTypes::kWrongAmountOfParameters}}
                          .ExtractValue());
    }
    repositories::QuestionRepostitory question_repo(pg_cluster_);
    auto question = question_repo.CreateQuestion(
        bimap_str_question_type.TryFindByFirst(question_type_str.value())
            .value(),
        test_id, question_text.value());
    return question.id;
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendCreateQuestion(userver::components::ComponentList& component_list) {
  component_list.Append<CreateQuestion>();
}

}  // namespace questions
}  // namespace handlers
}  // namespace mtquiz_service