#pragma once

#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>
#include <userver/utils/trivial_map.hpp>

namespace mtquiz_service {

enum class Roles { kOwner, kParticipant, kContributor };

}  // namespace mtquiz_service

template <>
struct userver::storages::postgres::io::CppToUserPg<mtquiz_service::Roles> {
  static constexpr DBTypeName postgres_name = "__pgtest.rainbow";
  static constexpr USERVER_NAMESPACE::utils::TrivialBiMap enumerators =
      [](auto selector) {
        return selector()
            .Case("Owner", mtquiz_service::Roles::kOwner)
            .Case("Participant", mtquiz_service::Roles::kParticipant)
            .Case("Contributor", mtquiz_service::Roles::kContributor);
      };
};