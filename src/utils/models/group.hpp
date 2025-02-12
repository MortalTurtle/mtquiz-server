#pragma once

#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace mtquiz_service {

struct Group {
  std::string id;
  std::string name;
  std::string description;
};

userver::formats::json::Value inline Serialize(
    const Group& group,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;
  item["id"] = group.id;
  item["name"] = group.name;
  item["description"] = group.description;
  return item.ExtractValue();
}

Group inline Parse(userver::formats::json::Value value,
                   userver::formats::parse::To<Group>) {
  return Group{value["id"].As<std::string>(), value["name"].As<std::string>(),
               value["description"].As<std::string>()};
}

}  // namespace mtquiz_service