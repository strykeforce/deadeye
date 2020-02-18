#include "config/filter_config.h"

#include <opencv2/core/mat.hpp>
#include "catch2/catch.hpp"

using namespace deadeye;

TEST_CASE("FilterConfig to JSON", "[pipeline]") {
  FilterConfig fc{{1.0, 2.0}, {2.5, 2.4}, {25.0, 25.1}};
  json j = fc;
  json expected = R"(
{"area":[1.0,2.0],"fullness":[2.5,2.4],"aspect":[25.0,25.1]}
)"_json;

  REQUIRE(j == expected);
}

TEST_CASE("FilterConfig from JSON", "[pipeline]") {
  auto expected =
      R"({"area":[1.0,2.0],"fullness":[2.5,2.4],"aspect":[25.0,25.1]})"_json;

  FilterConfig fc = expected;

  REQUIRE(fc.area[0] == 1.0);
  REQUIRE(fc.area[1] == 2.0);
  REQUIRE(fc.fullness[0] == 2.5);
  REQUIRE(fc.fullness[1] == 2.4);
  REQUIRE(fc.aspect[0] == 25.0);
  REQUIRE(fc.aspect[1] == 25.1);
}
