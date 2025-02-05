#include "Error.hpp"
#include <userver/formats/json/value_builder.hpp>

userver::formats::json::Value mtquiz_service::security::Serialize(
    const Error& error,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;
  item["description"] = error.description;
  item["type"] = error.type;
  return item.ExtractValue();
}