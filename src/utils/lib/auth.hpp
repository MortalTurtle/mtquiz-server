#pragma once

#include <optional>
#include <string_view>
#include <userver/server/http/http_request.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include "models/session.hpp"

namespace mtquiz_service {

namespace security {

constexpr std::string_view USER_TOKEN_HEADER_NAME = "X-User-Auth-Token";

std::optional<Session> GetSessionInfo(
    userver::storages::postgres::ClusterPtr pg_cluster,
    const userver::server::http::HttpRequest& request);

std::optional<Session> TryLogin(
    userver::storages::postgres::ClusterPtr pg_cluster,
    std::string_view username, std::string_view password);

}  // namespace security

}  // namespace mtquiz_service