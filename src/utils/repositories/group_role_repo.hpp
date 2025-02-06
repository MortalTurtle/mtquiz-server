#pragma once

#include <string_view>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include "models/group_role.hpp"
namespace mtquiz_service {

namespace repositories {

class GroupRoleRepository {
  userver::storages::postgres::ClusterPtr pg_cluster_;

 public:
  GroupRoleRepository(userver::storages::postgres::ClusterPtr pg_cluster);

  void AddRoleForUser(std::string_view user_id, std::string_view group_id,
                      Roles role);
};

}  // namespace repositories
}  // namespace mtquiz_service