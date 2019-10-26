#include "catch2/catch.hpp"
#include "link/center_target_data.h"

using namespace deadeye;

TEST_CASE("TargetData to JSON", "[link]") {
  TargetData td{"Z1", 1, true};

  json expected = R"({"id":"Z1","sn":1,"valid":true})"_json;
  json j = json::parse(td.Dump());
  REQUIRE(j == expected);
}

TEST_CASE("CenterTargetData to JSON", "[link]") {
  CenterTargetData td{"Z1", 1, true, 27, 67};

  json expected = R"({"id":"Z1","sn":1,"valid":true,"x":27,"y":67})"_json;
  json j = json::parse(td.Dump());
  REQUIRE(j == expected);
}
