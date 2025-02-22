#include "get_answers.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <optional>
#include <random>
#include <string>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/formats/json/inline.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/status_code.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include "lib/auth.hpp"
#include "models/Error.hpp"
#include "repositories/questions_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace answers {

namespace {

class GetAnswers final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-answers-get";

  GetAnswers(const userver::components::ComponentConfig& config,
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
    auto question_id = request.GetArg("questionId");
    bool get_false_answers = request.HasArg("falseAnswers");
    repositories::QuestionRepostitory question_repo(pg_cluster_);
    auto question = question_repo.GetQuestion(question_id);
    if (!question.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"No question found",
                          security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    auto answers = question_repo.GetAnswers(question_id, !get_false_answers);
    userver::formats::json::ValueBuilder item;
    std::for_each(answers.begin(), answers.end(),
                  [&](const Answer& ans) { item.PushBack(ans); });
    return answers.size() == 0
               ? "[]"
               : userver::formats::json::ToString(item.ExtractValue());
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendGetAnswers(userver::components::ComponentList& component_list) {
  component_list.Append<GetAnswers>();
}

}  // namespace answers
}  // namespace handlers
}  // namespace mtquiz_service