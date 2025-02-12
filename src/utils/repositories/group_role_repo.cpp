#include "group_role_repo.hpp"
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include "models/group_role.hpp"

namespace mtquiz_service {

namespace repositories {

GroupRoleRepository::GroupRoleRepository(
    userver::storages::postgres::ClusterPtr pg_cluster)
    : pg_cluster_(pg_cluster) {}

void GroupRoleRepository::AddRoleForUser(std::string_view user_id,
                                         std::string_view group_id,
                                         Roles role) {
  pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO quizdb.group_users(group_id, user_id, role) "
      "VALUES ($1, $2, $3) ON CONFLICT DO NOTHING",
      group_id, user_id, role);
}

std::optional<Roles> GroupRoleRepository::GetUserRoleInGroup(
    std::string_view user_id, std::string_view group_id) {
  auto res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           "SELECT role FROM quizdb.group_users WHERE user_id "
                           "= $1 AND group_id = $2",
                           user_id, group_id);
  if (res.IsEmpty()) return std::nullopt;
  return res[0]["role"].As<Roles>();
}

}  // namespace repositories

}  // namespace mtquiz_service