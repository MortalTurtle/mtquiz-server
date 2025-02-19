#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <functional>
#include <string>
#include <userver/formats/json/value_builder.hpp>

namespace mtquiz_service {

struct Answer {
  std::string question_id;
  std::string answer;
};

userver::formats::json::Value inline Serialize(
    const Answer& answer,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;
  item["questionId"] = answer.question_id;
  item["answer"] = answer.answer;
  return item.ExtractValue();
}

bool inline operator==(const Answer& lhs, const Answer& rhs) {
  return boost::algorithm::to_lower_copy(lhs.answer) == 
    boost::algorithm::to_lower_copy(rhs.answer);
}

Answer inline Parse(userver::formats::json::Value value,
                    userver::formats::parse::To<Answer>) {
  return Answer{value["questionId"].As<std::string>(),
                value["answer"].As<std::string>()};
}

}  // namespace mtquiz_service

namespace std {

template <>
struct hash<mtquiz_service::Answer> {
  size_t operator()(const mtquiz_service::Answer& ans) const noexcept {
    return hash<std::string>()(boost::algorithm::to_lower_copy(ans.answer));
  }
};

}  // namespace std