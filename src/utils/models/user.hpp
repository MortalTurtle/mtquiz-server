#pragma once

#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace mtquiz_service {

struct User {
  std::string id;
  std::string username;
  std::string password_hash;
  std::optional<std::string> group_id;
};

userver::formats::json::Value Serialize(const User& user,
    userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace mtquiz_service