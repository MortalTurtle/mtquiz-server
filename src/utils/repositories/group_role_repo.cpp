#include "group_role_repo.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
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
}  // namespace repositories

}  // namespace mtquiz_service