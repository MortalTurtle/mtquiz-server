#include "questions_repo.hpp"
#include <algorithm>
#include <limits>
#include <memory>
#include <optional>
#include <string_view>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include <vector>
#include "models/answer.hpp"
#include "models/question.hpp"

namespace mtquiz_service {

namespace repositories {

Question QuestionRepostitory::CreateQuestion(QuestionTypes type,
                                             std::string_view test_id,
                                             std::string_view question_text,
                                             int weight) {
  auto res = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO quizdb.test_questions(type, text, test_id, weight) "
      "VALUES ($1, $2, $3, $4) RETURNING *",
      type, question_text, test_id, weight);
  return res.AsSingleRow<Question>(userver::storages::postgres::kRowTag);
}

std::optional<Question> QuestionRepostitory::GetQuestion(
    std::string_view question_id) {
  auto res = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT * FROM quizdb.test_questions WHERE id = $1", question_id);
  if (res.IsEmpty()) return std::nullopt;
  return res.AsSingleRow<Question>(userver::storages::postgres::kRowTag);
}

std::vector<Question> QuestionRepostitory::GetAllQuestionInTest(
    std::string_view test_id, std::optional<int> limit,
    std::optional<int> offset) {
  auto res = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT * FROM quizdb.test_questions WHERE test_id = $1 ORDER BY "
      "created_ts DESC LIMIT $2 OFFSET $3",
      test_id,
      (limit.has_value() ? limit.value() : std::numeric_limits<int>::max()),
      (offset.has_value() ? offset.value() : 0));
  std::vector<Question> questions;
  std::for_each(res.begin(), res.end(),
                [&](const userver::storages::postgres::Row& row) {
                  questions.push_back(
                      row.As<Question>(userver::storages::postgres::kRowTag));
                });
  return questions;
}

void QuestionRepostitory::EditQuestion(std::string_view question_id,
                                       const std::optional<std::string>& text,
                                       std::optional<QuestionTypes> type,
                                       std::optional<int> weight) {
  auto transaction =
      pg_cluster_->Begin(userver::storages::postgres::TransactionOptions{
          userver::storages::postgres::TransactionOptions::kReadWrite});
  if (text.has_value())
    transaction.Execute(
        "UPDATE quizdb.test_questions SET text = $1 WHERE id = $2",
        text.value(), question_id);
  if (type.has_value())
    transaction.Execute(
        "UPDATE quizdb.test_questions SET type = $1 WHERE id = $2",
        type.value(), question_id);
  if (weight.has_value())
    transaction.Execute(
        "UPDATE quizdb.test_questions SET weight = $1 WHERE id = $2",
        weight.value(), question_id);
  transaction.Commit();
}

std::unordered_set<Answer> QuestionRepostitory::GetAnswers(
    std::string_view question_id, bool is_true_answers) {
  std::string table_name = TableName(is_true_answers);
  auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT * FROM quizdb." + table_name + " WHERE question_id = $1",
      question_id);
  std::unordered_set<Answer> answers;
  std::for_each(
      result.begin(), result.end(),
      [&](const userver::storages::postgres::Row& row) {
        answers.insert(row.As<Answer>(userver::storages::postgres::kRowTag));
      });
  return answers;
}

void QuestionRepostitory::AddAnswer(Answer answer, bool is_false_answer) {
  std::string table_name = TableName(!is_false_answer);
  pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO quizdb." + table_name +
          "(question_id, answer) VALUES($1, $2) ON CONFLICT DO NOTHING",
      answer.question_id, answer.answer);
}

void QuestionRepostitory::ClearAnswers(std::string_view question_id,
                                       bool are_true_answers) {
  std::string table_name = TableName(are_true_answers);
  pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "DELETE FROM quizdb." + table_name + " WHERE question_id = $1",
      question_id);
}

}  // namespace repositories

}  // namespace mtquiz_service