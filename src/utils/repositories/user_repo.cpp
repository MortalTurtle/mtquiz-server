#include "repositories/user_repo.hpp"
#include <optional>
#include <userver/crypto/hash.hpp>
#include <string_view>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include "models/user.hpp"

namespace mtquiz_service {

namespace repositories {

User UserRepository::CrateUser(
    std::string_view username,
    std::string_view password) {
    auto password_hash = userver::crypto::hash::Sha256(password);
    auto res = this->pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO quizdb.users(username, password_hash) "
        "VALUES ($1, $2) RETURNING *", username, password_hash);
    return res.AsSingleRow<User>(userver::storages::postgres::kRowTag);
}

std::optional<User> UserRepository::GetUserById(std::string_view id) {
    auto res = this->pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT * FROM quizdb.users WHERE id = $1", id
    );
    if (res.IsEmpty()) return std::nullopt;
    return res.AsSingleRow<User>(userver::storages::postgres::kRowTag);
}

std::optional<User> UserRepository::GetUserByUsername(std::string_view username) {
    auto res = this->pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT * FROM quizdb.users WHERE username = $1", username
    );
    if (res.IsEmpty()) return std::nullopt;
    return res.AsSingleRow<User>(userver::storages::postgres::kRowTag);
}

}
}