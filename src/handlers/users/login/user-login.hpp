#pragma once

#include <userver/components/component_list.hpp>

namespace mtquiz_service {

namespace handlers {
namespace users {
void AppendUserLogin(userver::components::ComponentList& component_list);

}  // namespace users
}  // namespace handlers
}  // namespace mtquiz_service
