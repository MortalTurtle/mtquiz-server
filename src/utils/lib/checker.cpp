#include "checker.hpp"
#include "lib/checker.hpp"

namespace mtquiz_service {
namespace checker {

bool CheckIfAnswersAdditionIsInvalid(const std::vector<Answer>& answers_to_add,
                                     const std::optional<Question>& question,
                                     const std::vector<Answer>& true_answers,
                                     bool are_false_answers) {
  auto& question_id = question.has_value() ? question->id : "";
  auto first_answer_not_for_this_question = std::find_if(
      answers_to_add.begin(), answers_to_add.end(),
      [&](const auto& ans) { return ans.question_id != question_id; });
  return !question.has_value() ||
         (question->type == QuestionTypes::kWrite && are_false_answers) ||
         (!are_false_answers &&
          question->type == QuestionTypes::kChooseSingle &&
          (true_answers.size() == 1 || answers_to_add.size() > 1)) ||
         first_answer_not_for_this_question != answers_to_add.end();
}

bool CheckIfAnswersReplacementIsInvalid(
    const std::vector<Answer>& answers_to_add,
    const std::optional<Question>& question, bool are_false_answers) {
  return CheckIfAnswersAdditionIsInvalid(
      answers_to_add, question, std::vector<Answer>(), are_false_answers);
}

}  // namespace checker

}  // namespace mtquiz_service