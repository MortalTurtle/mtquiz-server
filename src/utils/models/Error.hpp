#pragma once

#include <string>
#include <userver/formats/json/value.hpp>

namespace mtquiz_service {

namespace security {

struct Error {
  const std::string description;
  const std::string type;
};

struct ErrorTypes final {
  ErrorTypes() = delete;
  static inline std::string kWrongAmountOfParameters =
      "kWrongAmountOfParameters";
  static inline std::string kWrongParameterFormat = "kWrongParameterFormat";
  static inline std::string kInvalidParameters = "kInvalidParameters";
};

userver::formats::json::Value Serialize(
    const Error& error,
    userver::formats::serialize::To<userver::formats::json::Value>);
}  // namespace security

}  // namespace mtquiz_service