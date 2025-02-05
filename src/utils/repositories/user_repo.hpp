#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <userver/storages/postgres/cluster.hpp>
#include "models/session.hpp"
#include "models/user.hpp"

namespace mtquiz_service {

namespace repositories {

class UserRepository {
  const userver::storages::postgres::ClusterPtr pg_cluster_;

 public:
  UserRepository(const userver::storages::postgres::ClusterPtr pg_cluster)
      : pg_cluster_(pg_cluster) {}

  User CrateUser(std::string_view username, std::string_view password);

  std::optional<User> GetUserById(std::string_view id);

  std::optional<User> GetUserByUsername(std::string_view username);

  std::optional<User> GetUserBySession(const security::Session& session);

  // updates only username and password_hash
  void UpdateUserInfo(std::string_view user_id,
                      const std::optional<std::string>& username,
                      const std::optional<std::string> password);
};

}  // namespace repositories

}  // namespace mtquiz_service