#pragma once

#include <stdexcept>
#include <string_view>
namespace mtquiz_service {

namespace errors {

struct not_found_exception : std::runtime_error {
  not_found_exception(const std::string& error_msg)
      : std::runtime_error(error_msg) {}
};

}  // namespace errors

}  // namespace mtquiz_service