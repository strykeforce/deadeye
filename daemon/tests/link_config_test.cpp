#include "link_config.hpp"

#include <opencv2/core/mat.hpp>
#include "catch2/catch.hpp"

using namespace deadeye;

TEST_CASE("LinkConfig equality", "[link]") {
  LinkConfig lc1{"1234", 2767};
  LinkConfig lc2{"1234", 2767};
  REQUIRE(lc1 == lc2);

  LinkConfig lc3{"5678", 2767};
  REQUIRE(lc1 != lc3);
}

TEST_CASE("LinkConfig from JSON", "[link]") {
  json j;
  j[LinkConfig::kAddressKey] = "2767";
  j[LinkConfig::kPortKey] = 1;

  auto lc = j.get<LinkConfig>();
  REQUIRE(lc.address == j[LinkConfig::kAddressKey]);
  REQUIRE(lc.port == j[LinkConfig::kPortKey]);
}

TEST_CASE("LinkConfig to JSON", "[link]") {
  LinkConfig lc{"2767", 5};
  json j = lc;
  json expected;
  expected[LinkConfig::kAddressKey] = "2767";
  expected[LinkConfig::kPortKey] = 5;

  REQUIRE(j == expected);
}

TEST_CASE("LinkConfig New", "[link]") {
  LinkConfig expected{"2767", 7};
  json j = expected;
  auto val = nt::Value::MakeString(j.dump());
  auto lc = LinkConfig{val};
  REQUIRE(lc == expected);
}
