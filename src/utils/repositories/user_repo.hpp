#pragma once

#include <optional>
#include <string_view>
#include <userver/storages/postgres/cluster.hpp>
#include "models/user.hpp"

namespace mtquiz_service {

namespace repositories {

class UserRepository {
const userver::storages::postgres::ClusterPtr pg_cluster_;
public:
    UserRepository(const userver::storages::postgres::ClusterPtr pg_cluster) : 
        pg_cluster_(pg_cluster) {}

    User CrateUser(std::string_view username, std::string_view password);

    std::optional<User> GetUserById(std::string_view username);

    std::optional<User> GetUserByUsername(std::string_view id);
};

}

}