// Copyright (c) 2022 Stryke Force FRC 2767

// #include <iostream>
#include "config/capture_config.h"

#include "catch2/catch.hpp"

using ::deadeye::CaptureConfig;
using ::deadeye::CaptureType;

TEST_CASE("CaptureConfig config object", "[capture]") {
  auto j = R"(
  {
    "type": "test",
    "w": 320,
    "h": 180,
    "fps": 60,
    "config": {
        "pattern":"ball"
    }
  }
)"_json;

  auto cc = j.get<CaptureConfig>();
  REQUIRE(cc.type == CaptureType::test);
  REQUIRE(cc.width == 320);
  REQUIRE(cc.height == 180);
  REQUIRE(cc.frame_rate == 60);
  REQUIRE(cc.config["pattern"] == "ball");
}
