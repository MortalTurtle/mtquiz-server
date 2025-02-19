cmake_minimum_required(VERSION 3.12...3.31)
project(mtquiz_service CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DownloadUserver)

find_package(userver COMPONENTS core postgresql QUIET)
if(NOT userver_FOUND)
  # Tries TRY_DIR first, falls back to downloading userver from GitHub using CPM.
  download_userver(TRY_DIR third_party/userver)
endif()

userver_setup_environment()


# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
    src/handlers/answers/add_answer.cpp
    src/handlers/answers/replace_answers.cpp
    src/handlers/answers/submit_answers.cpp
    src/handlers/answers/get_answers.cpp
    src/handlers/questions/get_question.cpp
    src/handlers/questions/question_create.cpp
    src/handlers/questions/questions_get.cpp
    src/handlers/questions/edit_question.cpp
    src/handlers/tests/get_tests_for_group.cpp
    src/handlers/tests/create_test_for_group.cpp
    src/handlers/tests/get_test.cpp
    src/handlers/tests/edit_test.cpp
    src/handlers/groups/create_group.cpp
    src/handlers/groups/get_group.cpp
    src/handlers/groups/edit_group.cpp
    src/handlers/groups/join_group.cpp
    src/handlers/users/login/user-login.cpp
    src/handlers/users/add-user.cpp
    src/handlers/users/edit-user.cpp
    src/handlers/users/get-user.cpp
    src/utils/lib/auth.cpp
    src/utils/lib/checker.cpp
    src/utils/repositories/user_repo.cpp
    src/utils/repositories/group_repo.cpp
    src/utils/repositories/group_role_repo.cpp
    src/utils/repositories/tests_repo.cpp
    src/utils/repositories/questions_repo.cpp
    src/utils/models/Error.cpp
    src/utils/models/user.cpp
)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver::postgresql)
include_directories(utils src/utils)

# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# Unit Tests
add_executable(${PROJECT_NAME}_unittest
)
target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
add_google_tests(${PROJECT_NAME}_unittest)


# Benchmarks
add_executable(${PROJECT_NAME}_benchmark
)
target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver::ubench)
add_google_benchmark_tests(${PROJECT_NAME}_benchmark)

# Functional testing
userver_testsuite_add_simple()

# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX "${PREFIX_PATH}")
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
