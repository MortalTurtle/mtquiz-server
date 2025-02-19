#include "submit_answers.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
#include "models/Error.hpp"
#include "models/answer.hpp"
#include "models/question.hpp"
#include "repositories/group_role_repo.hpp"
#include "repositories/questions_repo.hpp"
#include "repositories/tests_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace answers {

namespace {

class SubmitAnswers final : public userver::server::handlers::HttpHandlerBase {
  int CheckAnswer(const Answer& ans, const Question& question,
                  repositories::QuestionRepostitory& questions_repo) const {
    auto answers = questions_repo.GetAnswers(question.id);
    if (answers.count(ans))
      return question.weight.has_value() ? question.weight.value() : 0;
    return 0;
  }

 public:
  static constexpr std::string_view kName = "handler-answers-submit";

  SubmitAnswers(const userver::components::ComponentConfig& config,
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
    auto request_body =
        userver::formats::json::FromString(request.RequestBody());
    auto test_id = request.GetArg("testId");
    repositories::TestsRepository test_repo(pg_cluster_);
    auto test = test_repo.GetTest(test_id);
    if (!test.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kNotFound);
      return {};
    }
    repositories::GroupRoleRepository role_repo(pg_cluster_);
    auto role = role_repo.GetUserRoleInGroup(session->user_id, test->group_id);
    if (!role.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kForbidden);
      return {};
    }
    std::vector<Answer> answers_to_check;
    std::for_each(request_body.begin(), request_body.end(),
                  [&](const userver::formats::json::Value& val) {
                    answers_to_check.push_back(val.As<Answer>());
                  });
    int totalScore = 0;
    auto questions = test_repo.GetAllQuestionsForTest(test_id);
    std::unordered_map<std::string, Question> question_id_to_obj;
    repositories::QuestionRepostitory question_repo(pg_cluster_);
    std::for_each(questions.begin(), questions.end(),
                  [&](const Question& question) {
                    question_id_to_obj[question.id] = question;
                  });
    auto first_ans_not_from_test = std::find_if(
        answers_to_check.begin(), answers_to_check.end(),
        [&](const Answer& ans) {
          if (!question_id_to_obj.count(ans.question_id)) return true;
          totalScore += CheckAnswer(ans, question_id_to_obj[ans.question_id],
                                    question_repo);
          return false;
        });
    if (first_ans_not_from_test != answers_to_check.end()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{
              "answers contain answer to test question from different test",
              security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    test_repo.SaveUserScore(session->user_id, test_id, totalScore);
    userver::formats::json::ValueBuilder item;
    item["userScore"] = std::to_string(totalScore);
    item["minScoreToPass"] = test->min_score.has_value()
                                 ? std::to_string(test->min_score.value())
                                 : "";
    return ToString(item.ExtractValue());
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendSubmitAnswers(userver::components::ComponentList& component_list) {
  component_list.Append<SubmitAnswers>();
}

}  // namespace answers
}  // namespace handlers
}  // namespace mtquiz_service