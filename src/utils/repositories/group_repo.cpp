
#include "repositories/group_repo.hpp"
#include <algorithm>
#include <optional>
#include <string_view>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/exceptions.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include <vector>
#include "lib/not_found_exception.hpp"
#include "models/group.hpp"
#include "models/group_role.hpp"
#include "models/test.hpp"
#include "models/test_result.hpp"
#include "repositories/group_role_repo.hpp"
namespace mtquiz_service {

namespace repositories {

Group GroupRepository::CreateGroup(std::string_view owner_id,
                                   std::string_view name,
                                   std::string_view description) {
  auto res = this->pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO quizdb.groups(name, description) VALUES ($1, $2) RETURNING "
      "*",
      name, description);
  auto group = res.AsSingleRow<Group>(userver::storages::postgres::kRowTag);
  GroupRoleRepository group_role_repo(pg_cluster_);
  group_role_repo.AddRoleForUser(owner_id, group.id, Roles::kOwner);
  return group;
}

std::optional<Group> GroupRepository::GetGroup(std::string_view group_id) {
  auto res = this->pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT * FROM quizdb.groups WHERE id = $1", group_id);
  if (res.IsEmpty()) return std::nullopt;
  return res.AsSingleRow<Group>(userver::storages::postgres::kRowTag);
}

void GroupRepository::JoinGroup(std::string_view user_id,
                                std::string_view group_id) {
  auto res = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "UPDATE quizdb.users SET group_id = $1 WHERE id = $2", group_id, user_id);
}

void GroupRepository::EditGroup(
    const Group& group, const std::optional<std::string>& new_name,
    const std::optional<std::string>& new_description) {
  this->pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "UPDATE quizdb.groups SET name = $1, description = $2 WHERE id = $3",
      (new_name.has_value() ? new_name.value() : group.name),
      (new_description.has_value() ? new_description.value()
                                   : group.description),
      group.id);
}

std::vector<TestResult> GroupRepository::GetUserTestResults(
    std::string_view user_id, std::string_view group_id, int limit, int offset,
    const std::optional<std::string>& test_id) {
  std::vector<TestResult> results;
  if (!test_id.has_value()) {
    auto res = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SElECT test_results.user_id, test_results.test_id, test_results.score "
        "FROM quizdb.test_results, quizdb.tests WHERE test_results.user_id = "
        "$1 AND tests.group_id = $2 AND tests.id = test_results.test_id ORDER "
        "BY test_results.finished_ts DESC LIMIT $3 OFFSET $4",
        user_id, group_id, limit, offset);
    std::for_each(res.begin(), res.end(),
                  [&](const userver::storages::postgres::Row& row) {
                    results.push_back(row.As<TestResult>(
                        userver::storages::postgres::kRowTag));
                  });
    return results;
  }
  auto res = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT * FROM quizdb.test_results WHERE user_id = $1 AND test_id = $2 "
      "ORDER BY finished_ts DESC LIMIT $3 OFFSET $4",
      user_id, test_id.value(), limit, offset);
  std::for_each(res.begin(), res.end(),
                [&](const userver::storages::postgres::Row& row) {
                  results.push_back(
                      row.As<TestResult>(userver::storages::postgres::kRowTag));
                });
  return results;
}

}  // namespace repositories

}  // namespace mtquiz_service