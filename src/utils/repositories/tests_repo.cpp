#include "tests_repo.hpp"
#include <algorithm>
#include <limits>
#include <optional>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include <vector>
#include "models/test.hpp"

namespace mtquiz_service {

namespace repositories {

std::vector<Test> TestsRepository::GetAllTestsForGroup(
    std::string_view group_id, std::optional<unsigned int> limit,
    std::optional<unsigned int> offset) {
  auto res = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT * FROM quizdb.tests WHERE group_id = $1 LIMIT $2 OFFSET $3",
      group_id,
      (limit.has_value() ? (int)limit.value()
                         : std::numeric_limits<int>::max()),
      (offset.has_value() ? (int)offset.value()
                          : std::numeric_limits<int>::max()));
  std::vector<Test> tests;
  std::for_each(
      res.begin(), res.end(), [&](const userver::storages::postgres::Row& row) {
        tests.push_back(row.As<Test>(userver::storages::postgres::kRowTag));
      });
  return tests;
}

std::optional<Test> TestsRepository::GetTest(std::string_view test_id) {
  auto res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           "SELECT * FROM quizdb.tests WHERE id = $1", test_id);
  if (res.IsEmpty()) return std::nullopt;
  return res.AsSingleRow<Test>(userver::storages::postgres::kRowTag);
}

Test TestsRepository::CreateTest(std::string_view test_name,
                                 std::string_view test_description,
                                 std::string_view owner_id,
                                 std::string_view group_id) {
  auto res = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO quizdb.tests(group_id, owner_id, name, description) VALUES "
      "($1, $2, $3, $4) RETURNING *",
      group_id, owner_id, test_name, test_description);
  return res.AsSingleRow<Test>(userver::storages::postgres::kRowTag);
}

}  // namespace repositories

}  // namespace mtquiz_service