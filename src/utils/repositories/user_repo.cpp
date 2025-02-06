#include "repositories/user_repo.hpp"
#include <optional>
#include <string_view>
#include <userver/crypto/hash.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include "models/user.hpp"

namespace mtquiz_service {

namespace repositories {

User UserRepository::CrateUser(std::string_view username,
                               std::string_view password) {
  auto password_hash = userver::crypto::hash::Sha256(password);
  auto res = this->pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO quizdb.users(username, password_hash) "
      "VALUES ($1, $2) RETURNING *",
      username, password_hash);
  return res.AsSingleRow<User>(userver::storages::postgres::kRowTag);
}

std::optional<User> UserRepository::GetUserById(std::string_view id) {
  auto res = this->pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT * FROM quizdb.users WHERE id = $1", id);
  if (res.IsEmpty()) return std::nullopt;
  return res.AsSingleRow<User>(userver::storages::postgres::kRowTag);
}

std::optional<User> UserRepository::GetUserByUsername(
    std::string_view username) {
  auto res = this->pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT * FROM quizdb.users WHERE username = $1", username);
  if (res.IsEmpty()) return std::nullopt;
  return res.AsSingleRow<User>(userver::storages::postgres::kRowTag);
}

std::optional<User> UserRepository::GetUserBySession(
    const security::Session& session) {
  auto res = this->pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT users.id, users.username, users.password_hash, users.group_id "
      "FROM quizdb.users, quizdb.session_tokens WHERE session_tokens.id = $1 "
      "AND "
      "session_tokens.user_id = users.id",
      session.id);
  if (res.IsEmpty()) return std::nullopt;
  return res.AsSingleRow<User>(userver::storages::postgres::kRowTag);
}

void UserRepository::UpdateUserInfo(
    std::string_view user_id, const std::optional<std::string>& username_new,
    const std::optional<std::string> password_new) {
  if (username_new.has_value()) {
    this->pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE quizdb.users SET username = $1 WHERE id = $2 "
        "AND username != $1",
        username_new.value(), user_id);
  }
  if (!password_new.has_value()) return;
  auto new_hash = userver::crypto::hash::Sha256(password_new.value());
  this->pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "UPDATE quizdb.users SET password_hash = $1 WHERE id = $2 ", new_hash,
      user_id);
}

}  // namespace repositories
}  // namespace mtquiz_service