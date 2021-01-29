#include <opencv2/core/types.hpp>

#include "catch2/catch.hpp"
#include "link/target_list_target_data.h"
#include "link/upright_target_data.h"

using namespace deadeye;

TEST_CASE("TargetData to JSON", "[link]") {
  TargetData td{"Z1", 1, true};

  json expected = R"({"id":"Z1","sn":1,"v":true})"_json;
  json j = json::parse(td.Dump());
  REQUIRE(j == expected);
}

TEST_CASE("UprightTargetData to JSON", "[link]") {
  UprightTargetData td{"Z1", 1, true, cv::Rect{0, 0, 100, 100}};

  json expected = R"({"d":[0,0,100,100,50,50],"id":"Z1","sn":1,"v":true})"_json;
  json j = json::parse(td.Dump());
  REQUIRE(j == expected);
}

TEST_CASE("TargetListTargetData to JSON", "[link]") {
  TargetList targets;
  TargetListTargetData td1{"Z1", 1, false, targets};

  json expected = R"({"d":[],"id":"Z1","sn":1,"v":false})"_json;
  json j = json::parse(td1.Dump());
  REQUIRE(j == expected);

  targets = {{1, 2, 3, 4, 5}, {10, 20, 30, 40, 50}};
  TargetListTargetData td2{"Z1", 1, true, targets};

  expected =
      R"({"d":[[1,2,3,4,5],[10,20,30,40,50]],"id":"Z1","sn":1,"v":true})"_json;
  j = json::parse(td2.Dump());
  REQUIRE(j == expected);
}

TEST_CASE("TargetListTargetData serialized size", "[link]") {
  TargetList targets;
  for (size_t i = 0; i < 20; i++) {
    targets.push_back({999, 999, 999, 999, 9999});
  }
  TargetListTargetData td1{"Z1", 123'456'789, true, targets};
  auto serialized = td1.Dump();
  REQUIRE(serialized.size() < 512);

  for (size_t i = 0; i < 5; i++) {
    targets.push_back({999, 999, 999, 999, 9999});
  }
  TargetListTargetData td2{"Z1", 123'456'789, true, targets};
  serialized = td2.Dump();
  json expected =
      R"({"d":[[-1,-1,-1,-1,-1]],"id":"Z1","sn":123456789,"v":false})"_json;
  json j = json::parse(td2.Dump());
  REQUIRE(j == expected);
}

// 320x180 test pattern hue = [90, 90]
TEST_CASE("Offset test - Cyan", "[link") {
  cv::Rect bb{cv::Point{91, 30}, cv::Point{137, 150}};
  UprightTargetData td{"Z1", 1, true, bb};
  REQUIRE(td.bb == bb);
}
