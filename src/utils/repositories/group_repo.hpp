#pragma once

#include <optional>
#include <string_view>
#include <userver/storages/postgres/cluster.hpp>
#include "models/group.hpp"
namespace mtquiz_service {

namespace repositories {

class GroupRepository {
  const userver::storages::postgres::ClusterPtr pg_cluster_;

 public:
  GroupRepository(const userver::storages::postgres::ClusterPtr pg_cluster)
      : pg_cluster_(pg_cluster) {}

  Group CreateGroup(std::string_view owner_id, std::string_view name,
                    std::string_view description);

  std::optional<Group> GetGroup(std::string_view group_id);
};

}  // namespace repositories
}  // namespace mtquiz_service