#include "questions_get.hpp"

#include <fmt/format.h>

#include <algorithm>
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
#include "models/question.hpp"
#include "repositories/questions_repo.hpp"
#include "repositories/tests_repo.hpp"
#include "repositories/user_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace questions {

namespace {

class GetQuestions final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-questions-get";

  GetQuestions(const userver::components::ComponentConfig& config,
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
    repositories::TestsRepository test_repo(pg_cluster_);
    auto test = test_repo.GetTest(test_id);
    repositories::UserRepository user_repo(pg_cluster_);
    std::optional<int> limit =
        request.HasArg("limit") ? std::optional(stoi(request.GetArg("limit")))
                                : std::nullopt;
    std::optional<int> offset =
        request.HasArg("offset") ? std::optional(stoi(request.GetArg("offset")))
                                 : std::nullopt;
    auto user = user_repo.GetUserById(session->user_id);
    if (!test.has_value() || (limit.has_value() && limit.value() < 0) ||
        (offset.has_value() && offset.value() < 0)) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"No test found or negative limit or offset",
                          security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    if (!user->group_id.has_value() ||
        user->group_id.value() != test->group_id) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kForbidden);
      return "You must join the group to get all questions from test";
    }
    repositories::QuestionRepostitory question_repo(pg_cluster_);
    auto questions = question_repo.GetAllQuestionInTest(test_id, limit, offset);
    userver::formats::json::ValueBuilder item;
    std::for_each(
        questions.begin(), questions.end(), [&](const Question& question) {
          item.PushBack(userver::formats::json::ValueBuilder{question});
        });
    return ToString(item.ExtractValue());
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendQuestionsGet(userver::components::ComponentList& component_list) {
  component_list.Append<GetQuestions>();
}

}  // namespace questions
}  // namespace handlers
}  // namespace mtquiz_service