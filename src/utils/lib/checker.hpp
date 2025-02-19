#pragma once

#include "models/answer.hpp"
#include "models/question.hpp"

namespace mtquiz_service {
namespace checker {

bool CheckIfAnswersAdditionIsInvalid(const std::vector<Answer>& answers_to_add,
                                     const std::optional<Question>& question,
                                     const std::vector<Answer>& true_answers,
                                     bool are_false_answers);

bool CheckIfAnswersReplacementIsInvalid(
    const std::vector<Answer>& answers_to_add,
    const std::optional<Question>& question, bool are_false_answers);

}  // namespace checker
}  // namespace mtquiz_service