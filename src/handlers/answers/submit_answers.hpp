#pragma once

#include <userver/components/component_list.hpp>

namespace mtquiz_service {

namespace handlers {

namespace answers {

void AppendSubmitAnswers(userver::components::ComponentList& component_list);

}  // namespace answers
}  // namespace handlers
}  // namespace mtquiz_service
