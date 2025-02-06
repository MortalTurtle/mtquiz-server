#pragma once

#include <string>

namespace mtquiz_service {

namespace security {

struct Session {
  std::string id;
  std::string user_id;
};

}  // namespace security
}  // namespace mtquiz_service