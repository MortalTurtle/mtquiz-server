#pragma once

#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>
#include <userver/utils/trivial_map.hpp>

namespace mtquiz_service {
enum class QuestionTypes { kChooseSingle, kChooseMultiple, kWrite };

static constexpr userver::utils::TrivialBiMap bimap_str_question_type =
    [](auto selector) {
      return selector()
          .Case("ChooseSingle", mtquiz_service::QuestionTypes::kChooseSingle)
          .Case("ChooseMultiple",
                mtquiz_service::QuestionTypes::kChooseMultiple)
          .Case("Write", mtquiz_service::QuestionTypes::kWrite);
    };

struct QuestionTypeDescription {
  QuestionTypes type;
  std::string name;
  std::string description;
};
}  // namespace mtquiz_service

template <>
struct userver::storages::postgres::io::CppToUserPg<
    mtquiz_service::QuestionTypes> {
  static constexpr DBTypeName postgres_name = "quizdb.question_type";
  static constexpr USERVER_NAMESPACE::utils::TrivialBiMap enumerators =
      mtquiz_service::bimap_str_question_type;
};