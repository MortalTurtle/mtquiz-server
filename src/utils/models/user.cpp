#include "user.hpp"

userver::formats::json::Value Serialize(
    const mtquiz_service::User& user,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;
  item["id"] = user.id;
  item["username"] = user.username;
  item["password_hash"] = user.password_hash;
  if (user.group_id.has_value()) item["group_id"] = user.group_id.value();
  return item.ExtractValue();
}