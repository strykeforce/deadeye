// Copyright (c) 2022 Stryke Force FRC 2767

#include "config.h"

#include <catch2/catch.hpp>
#include <string>

using Catch::Matchers::Equals;

TEST_CASE("CameraId configured", "[config]") {
  std::string expected{DEADEYE_UNIT "3"};
  REQUIRE(deadeye::CameraId(3) == expected);
}

TEST_CASE("CameraControlTablePath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3"};
  REQUIRE_THAT(expected, Equals(DE_CAMERA_CONTROL_TABLE("3")));
  REQUIRE(deadeye::CameraControlTablePath(3) == expected);
}

TEST_CASE("LightsControlTablePath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/Light"};
  REQUIRE_THAT(expected, Equals(DE_LIGHTS_CONTROL_TABLE("3")));
  REQUIRE(deadeye::LightsControlTablePath(3) == expected);
}

TEST_CASE("CaptureConfigEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/Capture"};
  REQUIRE_THAT(expected, Equals(DE_CAPTURE_CONFIG_ENTRY("3")));
  REQUIRE(deadeye::CaptureConfigEntryPath(3) == expected);
}

TEST_CASE("PipelineConfigEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/Pipeline"};
  REQUIRE_THAT(expected, Equals(DE_PIPELINE_CONFIG_ENTRY("3")));
  REQUIRE(deadeye::PipelineConfigEntryPath(3) == expected);
}

TEST_CASE("StreamConfigEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/Stream"};
  REQUIRE_THAT(expected, Equals(DE_STREAM_CONFIG_ENTRY("3")));
  REQUIRE(deadeye::StreamConfigEntryPath(3) == expected);
}

TEST_CASE("LogEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/Log"};
  REQUIRE(deadeye::LogEntryPath(3) == expected);
}

TEST_CASE("StateEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/State"};
  REQUIRE(deadeye::StateEntryPath(3) == expected);
}

TEST_CASE("LinkEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/Link"};
  REQUIRE_THAT(expected, Equals(DE_LINK_ENTRY));
}
