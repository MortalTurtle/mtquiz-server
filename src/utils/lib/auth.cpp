#include "auth.hpp"
#include <optional>
#include <string>
#include <userver/crypto/hash.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include "models/session.hpp"

namespace mtquiz_service {

namespace security {
std::optional<Session> GetSessionInfo(
    userver::storages::postgres::ClusterPtr pg_cluster,
    const userver::server::http::HttpRequest& request) {
  if (!request.HasHeader(USER_TOKEN_HEADER_NAME)) return std::nullopt;

  auto id = request.GetHeader(USER_TOKEN_HEADER_NAME);
  auto res =
      pg_cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                          "SELECT * FROM quizdb.session_tokens "
                          "WHERE id = $1",
                          id);
  if (res.IsEmpty()) return std::nullopt;
  return res.AsSingleRow<Session>(userver::storages::postgres::kRowTag);
}

std::optional<Session> TryLogin(
    userver::storages::postgres::ClusterPtr pg_cluster,
    std::string_view username, std::string_view password) {
  auto hash = userver::crypto::hash::Sha256(password);
  auto res = pg_cluster->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT id FROM quizdb.users WHERE username = $1 AND password_hash = $2",
      username, hash);
  if (res.IsEmpty()) return std::nullopt;
  std::string user_id = res[0]["id"].As<std::string>();
  res = pg_cluster->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO quizdb.session_tokens(user_id) VALUES ($1) RETURNING *",
      user_id);
  return res.AsSingleRow<Session>(userver::storages::postgres::kRowTag);
}
}  // namespace security
}  // namespace mtquiz_service