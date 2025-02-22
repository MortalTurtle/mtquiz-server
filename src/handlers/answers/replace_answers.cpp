#include "replace_answers.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <optional>
#include <string>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/status_code.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include "lib/auth.hpp"
#include "lib/checker.hpp"
#include "models/Error.hpp"
#include "models/answer.hpp"
#include "models/group_role.hpp"
#include "repositories/group_role_repo.hpp"
#include "repositories/questions_repo.hpp"
#include "repositories/tests_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace answers {

namespace {

class ReplaceAnswers final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-answers-replace";

  ReplaceAnswers(const userver::components::ComponentConfig& config,
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
    repositories::QuestionRepostitory question_repo(pg_cluster_);
    repositories::TestsRepository test_repo(pg_cluster_);
    repositories::GroupRoleRepository role_repo(pg_cluster_);
    auto are_false_answers = request.HasArg("falseAnswers");
    auto question_id = request.GetArg("questionId");
    auto request_body =
        userver::formats::json::FromString(request.RequestBody());
    std::vector<Answer> answers_to_add;
    std::for_each(request_body.begin(), request_body.end(),
                  [&](const userver::formats::json::Value& ans) {
                    answers_to_add.push_back(ans.As<Answer>());
                  });
    auto question = question_repo.GetQuestion(question_id);
    if (checker::CheckIfAnswersReplacementIsInvalid(answers_to_add, question,
                                                    are_false_answers)) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"No question found or this question does not support "
                          "this many of answers of this type",
                          security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    auto test = test_repo.GetTest(question->test_id);
    auto role = role_repo.GetUserRoleInGroup(session->user_id, test->group_id);
    if (!role.has_value() || role == Roles::kParticipant ||
        (role == Roles::kContributor && test->owner_id != session->user_id)) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kForbidden);
      return {};
    }
    question_repo.ClearAnswers(question_id, !are_false_answers);
    std::for_each(answers_to_add.begin(), answers_to_add.end(),
                  [&](const auto& ans) {
                    question_repo.AddAnswer(ans, are_false_answers);
                  });
    return {};
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendReplaceAnswers(userver::components::ComponentList& component_list) {
  component_list.Append<ReplaceAnswers>();
}

}  // namespace answers
}  // namespace handlers
}  // namespace mtquiz_service