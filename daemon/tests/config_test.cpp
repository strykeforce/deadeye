#include "config.h"
#include <string>
#include <catch2/catch.hpp>

using namespace deadeye;
using Catch::Matchers::Equals;

TEST_CASE("CameraId configured", "[config]") {
  std::string expected{DEADEYE_UNIT "3"};
  REQUIRE(CameraId(3) == expected);
}

TEST_CASE("CameraControlTablePath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3"};
  REQUIRE_THAT(expected, Equals(DE_CAMERA_CONTROL_TABLE("3")));
  REQUIRE(CameraControlTablePath(3) == expected);
}

TEST_CASE("LightsControlTablePath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/Light"};
  REQUIRE_THAT(expected, Equals(DE_LIGHTS_CONTROL_TABLE("3")));
  REQUIRE(LightsControlTablePath(3) == expected);
}

TEST_CASE("CaptureConfigEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/Capture"};
  REQUIRE_THAT(expected, Equals(DE_CAPTURE_CONFIG_ENTRY("3")));
  REQUIRE(CaptureConfigEntryPath(3) == expected);
}

TEST_CASE("PipelineConfigEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/Pipeline"};
  REQUIRE_THAT(expected, Equals(DE_PIPELINE_CONFIG_ENTRY("3")));
  REQUIRE(PipelineConfigEntryPath(3) == expected);
}

TEST_CASE("StreamConfigEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/Stream"};
  REQUIRE_THAT(expected, Equals(DE_STREAM_CONFIG_ENTRY("3")));
  REQUIRE(StreamConfigEntryPath(3) == expected);
}

TEST_CASE("LogEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/Log"};
  REQUIRE(LogEntryPath(3) == expected);
}

TEST_CASE("StateEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/" DEADEYE_UNIT "/3/State"};
  REQUIRE(StateEntryPath(3) == expected);
}

TEST_CASE("LinkEntryPath configured", "[config]") {
  std::string expected{"/Deadeye/Link"};
  REQUIRE_THAT(expected, Equals(DE_LINK_ENTRY));
}
