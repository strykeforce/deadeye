// Copyright (c) 2022 Stryke Force FRC 2767

#include "config/link_config.h"

#include <opencv2/core/mat.hpp>

#include "catch2/catch.hpp"

using ::deadeye::LinkConfig;
using json = nlohmann::json;

TEST_CASE("LinkConfig equality", "[link]") {
  LinkConfig lc1{"1234", 2767, true};
  LinkConfig lc2{"1234", 2767, true};
  REQUIRE(lc1 == lc2);

  LinkConfig lc3{"1234", 2767, false};
  REQUIRE(lc1 != lc3);
}

TEST_CASE("LinkConfig from JSON", "[link]") {
  json j;
  j[LinkConfig::kAddressKey] = "2767";
  j[LinkConfig::kPortKey] = 1;
  j[LinkConfig::kEnabledKey] = true;

  auto lc = j.get<LinkConfig>();
  REQUIRE(lc.address == j[LinkConfig::kAddressKey]);
  REQUIRE(lc.port == j[LinkConfig::kPortKey]);
  REQUIRE(lc.enabled == j[LinkConfig::kEnabledKey]);
}

TEST_CASE("LinkConfig to JSON", "[link]") {
  LinkConfig lc{"2767", 5, true};
  json j = lc;
  json expected;
  expected[LinkConfig::kAddressKey] = "2767";
  expected[LinkConfig::kPortKey] = 5;
  expected[LinkConfig::kEnabledKey] = true;

  REQUIRE(j == expected);
}

TEST_CASE("LinkConfig New", "[link]") {
  LinkConfig expected{"2767", 7, true};
  json j = expected;
  auto lc = LinkConfig{j};
  REQUIRE(lc == expected);
}
