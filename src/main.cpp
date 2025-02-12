#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include "handlers/groups/create_group.hpp"
#include "handlers/groups/edit_group.hpp"
#include "handlers/groups/get_group.hpp"
#include "handlers/groups/join_group.hpp"
#include "handlers/tests/create_test_for_group.hpp"
#include "handlers/tests/edit_test.hpp"
#include "handlers/tests/get_test.hpp"
#include "handlers/tests/get_tests_for_group.hpp"
#include "handlers/users/add-user.hpp"
#include "handlers/users/edit-user.hpp"
#include "handlers/users/get-user.hpp"
#include "handlers/users/login/user-login.hpp"

int main(int argc, char* argv[]) {
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::Ping>()
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::components::HttpClient>()
          .Append<userver::server::handlers::TestsControl>()
          .Append<userver::components::Postgres>("mtquiz-db-1")
          .Append<userver::clients::dns::Component>();

  mtquiz_service::handlers::groups::AppendCreateGroup(component_list);
  mtquiz_service::handlers::groups::AppendGetGroup(component_list);
  mtquiz_service::handlers::groups::AppendEditGroup(component_list);
  mtquiz_service::handlers::groups::AppendJoinGroup(component_list);
  mtquiz_service::handlers::users::AppendAddUser(component_list);
  mtquiz_service::handlers::users::AppendEditUser(component_list);
  mtquiz_service::handlers::users::AppendGetUser(component_list);
  mtquiz_service::handlers::users::AppendUserLogin(component_list);
  mtquiz_service::handlers::tests::AppendCreateTest(component_list);
  mtquiz_service::handlers::tests::AppendGetTests(component_list);
  mtquiz_service::handlers::tests::AppendGetTest(component_list);
  mtquiz_service::handlers::tests::AppendEditTest(component_list);

  return userver::utils::DaemonMain(argc, argv, component_list);
}
