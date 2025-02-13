#include "edit_question.hpp"
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
#include "models/group_role.hpp"
#include "models/question_type.hpp"
#include "repositories/group_role_repo.hpp"
#include "repositories/questions_repo.hpp"
#include "repositories/tests_repo.hpp"
#include "repositories/user_repo.hpp"

namespace mtquiz_service {

namespace handlers {
namespace questions {

namespace {

class EditQuestion final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-question-edit";

  EditQuestion(const userver::components::ComponentConfig& config,
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
    auto question_id = request.GetPathArg("id");
    repositories::TestsRepository test_repo(pg_cluster_);
    repositories::UserRepository user_repo(pg_cluster_);
    repositories::GroupRoleRepository role_repo(pg_cluster_);
    auto user = user_repo.GetUserById(session->user_id);
    auto test = test_repo.GetTest(test_id);
    auto request_body =
        userver::formats::json::FromString(request.RequestBody());
    auto type_str = request_body["type"].As<std::optional<std::string>>();
    auto text = request_body["text"].As<std::optional<std::string>>();
    auto weight = request_body["weight"].As<std::optional<int>>();
    if (!test.has_value() ||
        (!type_str.has_value() && !text.has_value() && !weight.has_value()) ||
        (weight.has_value() && weight.value() < 0)) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kBadRequest);
      return ToString(userver::formats::json::ValueBuilder{
          security::Error{"test not found or no parameters passed",
                          security::ErrorTypes::kInvalidParameters}}
                          .ExtractValue());
    }
    auto group_role =
        role_repo.GetUserRoleInGroup(session->user_id, test->group_id);
    if (!user->group_id.has_value() || test->group_id != user->group_id ||
        !group_role.has_value() || group_role == Roles::kParticipant ||
        (group_role == Roles::kContributor &&
         test->owner_id != session->user_id)) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kForbidden);
      return "You must be a part of the group of test belongs to wrong group "
             "or your privileges are insufficient";
    }
    repositories::QuestionRepostitory question_repo(pg_cluster_);
    auto question = question_repo.GetQuestion(question_id);
    if (!question.has_value()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::http::kNotFound);
      return {};
    }
    auto type = type_str.has_value()
                    ? bimap_str_question_type.TryFindByFirst(type_str.value())
                    : std::nullopt;
    question_repo.EditQuestion(question_id, text, type, weight);
    return {};
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendEditQuestion(userver::components::ComponentList& component_list) {
  component_list.Append<EditQuestion>();
}

}  // namespace questions
}  // namespace handlers
}  // namespace mtquiz_service