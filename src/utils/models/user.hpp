#pragma once

#include <optional>
#include <string>

namespace mtquiz_service {

struct User {
  std::string id;
  std::string username;
  std::string password_hash;
  std::optional<std::string> group_id;
};

}  // namespace mtquiz_service