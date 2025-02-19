#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <vector>
#include "models/answer.hpp"
#include "models/question.hpp"
#include "models/question_type.hpp"
namespace mtquiz_service {

namespace repositories {

class QuestionRepostitory {
  userver::storages::postgres::ClusterPtr pg_cluster_;

  static std::string inline TableName(bool is_true_answers) {
    return is_true_answers ? "question_answers" : "question_false_answers";
  }

 public:
  QuestionRepostitory(userver::storages::postgres::ClusterPtr pg_cluster)
      : pg_cluster_(pg_cluster) {}

  Question CreateQuestion(QuestionTypes type, std::string_view test_id,
                          std::string_view question_text);

  std::optional<Question> GetQuestion(std::string_view question_id);

  std::vector<Question> GetAllQuestionInTest(std::string_view test_id,
                                             std::optional<int> limit,
                                             std::optional<int> offset);

  void EditQuestion(std::string_view question_id,
                    const std::optional<std::string>& text,
                    std::optional<QuestionTypes> type,
                    std::optional<int> weight);

  std::unordered_set<Answer> GetAnswers(std::string_view question_id,
                                        bool is_true_answers = true);

  void AddAnswer(Answer answer, bool is_false_answer);

  void ClearAnswers(std::string_view question_id, bool are_true_answers = true);
};

}  // namespace repositories
}  // namespace mtquiz_service