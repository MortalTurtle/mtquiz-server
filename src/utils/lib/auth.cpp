#include "auth.hpp"
#include <optional>
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
}  // namespace security
}  // namespace mtquiz_service