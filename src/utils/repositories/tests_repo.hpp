#pragma once

#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <vector>
#include "models/question.hpp"
#include "models/test.hpp"

namespace mtquiz_service {

namespace repositories {

class TestsRepository {
  userver::storages::postgres::ClusterPtr pg_cluster_;

 public:
  TestsRepository(userver::storages::postgres::ClusterPtr pg_cluster)
      : pg_cluster_(pg_cluster) {}

  std::vector<Test> GetAllTestsForGroup(
      std::string_view group_id,
      std::optional<unsigned int> limit = std::nullopt,
      std::optional<unsigned int> offset = std::nullopt);

  std::optional<Test> GetTest(std::string_view test_id);

  Test CreateTest(std::string_view test_name, std::string_view test_description,
                  std::string_view owner_id, std::string_view group_id);
  void EditTest(std::string_view test_id,
                const std::optional<std::string>& name,
                const std::optional<std::string>& description,
                const std::optional<int>& min_score);

  std::vector<Question> GetAllQuestionsForTest(std::string_view test_id);

  void SaveUserScore(std::string_view user_id, std::string_view test_id,
                     int score);
};
}  // namespace repositories

}  // namespace mtquiz_service