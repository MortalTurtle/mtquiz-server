#pragma once

#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <userver/storages/postgres/cluster.hpp>
#include "models/group.hpp"
#include "models/test_result.hpp"
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

  void JoinGroup(std::string_view user_id, std::string_view group_id);

  void EditGroup(const Group& group, const std::optional<std::string>& new_name,
                 const std::optional<std::string>& new_description);

  std::vector<TestResult> GetUserTestResults(
      std::string_view user_id, std::string_view group_id,
      int limit = std::numeric_limits<int>::max(), int offset = 0,
      const std::optional<std::string>& test_id = std::nullopt);
};

}  // namespace repositories
}  // namespace mtquiz_service