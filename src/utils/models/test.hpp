#pragma once

#include <optional>
#include <string>
#include <userver/formats/json/value_builder.hpp>

namespace mtquiz_service {

struct Test {
  std::string id;
  std::string group_id;
  std::string owner_id;
  std::string name;
  std::optional<int> min_score;
  std::string description;
};

userver::formats::json::Value inline Serialize(
    const mtquiz_service::Test& test,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;
  item["id"] = test.id;
  item["groupId"] = test.group_id;
  item["ownerId"] = test.owner_id;
  item["name"] = test.name;
  if (test.min_score.has_value()) item["minScore"] = test.min_score.value();
  item["description"] = test.description;
  return item.ExtractValue();
}

}  // namespace mtquiz_service