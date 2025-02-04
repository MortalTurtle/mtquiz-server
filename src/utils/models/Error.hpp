#pragma once

#include <string>
#include <userver/formats/json/value.hpp>

namespace mtquiz_service {

namespace security {

struct Error {
    const std::string description;
    const std::string type;
};

userver::formats::json::Value Serialize(const Error& error,
    userver::formats::serialize::To<userver::formats::json::Value>);
}

}