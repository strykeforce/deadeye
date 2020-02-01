#include <opencv2/core/types.hpp>
#include "catch2/catch.hpp"
#include "link/upright_target_data.h"

using namespace deadeye;

TEST_CASE("TargetData to JSON", "[link]") {
  TargetData td{"Z1", 1, true};

  json expected = R"({"id":"Z1","sn":1,"valid":true})"_json;
  json j = json::parse(td.Dump());
  REQUIRE(j == expected);
}

TEST_CASE("CenterTargetData to JSON", "[link]") {
  UprightTargetData td{"Z1", 1, true, cv::Rect{0, 0, 100, 100},
                       cv::Point{50, 50}};

  json expected =
      R"({"id":"Z1","sn":1,"valid":true,"x":0,"y":0,"bx":100,"by":100,"tx":0,"ty":0})"_json;
  json j = json::parse(td.Dump());
  REQUIRE(j == expected);
}

// 320x180 test pattern hue = [90, 90]
TEST_CASE("Offset test - Cyan", "[link") {
  cv::Rect bb{cv::Point{91, 30}, cv::Point{137, 150}};
  cv::Point center{160, 90};
  UprightTargetData td{"Z1", 1, true, bb, center};
  REQUIRE(td.bb == bb);
}