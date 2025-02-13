#pragma once

#include <optional>
#include <string>
#include <userver/formats/json/value_builder.hpp>
#include "models/question_type.hpp"

namespace mtquiz_service {

struct Question {
  std::string id;
  std::string test_id;
  QuestionTypes type;
  std::optional<int> weight;
  std::string text;
};

userver::formats::json::Value inline Serialize(
    const Question& question,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;
  item["id"] = question.id;
  item["testId"] = question.test_id;
  item["type"] = bimap_question_type_str.TryFindBySecond(question.type).value();
  if (question.weight.has_value()) item["weight"] = question.weight.value();
  item["text"] = question.text;
  return item.ExtractValue();
}

}  // namespace mtquiz_service