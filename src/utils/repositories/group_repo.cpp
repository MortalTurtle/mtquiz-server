
#include "repositories/group_repo.hpp"
#include <optional>
#include <string_view>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include "models/group.hpp"
#include "models/group_role.hpp"
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

}  // namespace repositories

}  // namespace mtquiz_service