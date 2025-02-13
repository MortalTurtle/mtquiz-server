#include "questions_repo.hpp"
#include <algorithm>
#include <limits>
#include <memory>
#include <optional>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include "models/question.hpp"

namespace mtquiz_service {

namespace repositories {

Question QuestionRepostitory::CreateQuestion(QuestionTypes type,
                                             std::string_view test_id,
                                             std::string_view question_text) {
  auto res = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO quizdb.test_questions(type, text, test_id) "
      "VALUES ($1, $2, $3) RETURNING *",
      type, question_text, test_id);
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

}  // namespace repositories

}  // namespace mtquiz_service