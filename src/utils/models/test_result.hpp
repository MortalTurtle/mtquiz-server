#pragma once

#include <string>
#include <userver/formats/json/value_builder.hpp>

namespace mtquiz_service {

struct TestResult {
  std::string user_id;
  std::string test_id;
  int score;
};

userver::formats::json::Value inline Serialize(
    const TestResult& res,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;
  item["userId"] = res.user_id;
  item["testId"] = res.test_id;
  item["score"] = res.score;
  return item.ExtractValue();
}

}  // namespace mtquiz_service