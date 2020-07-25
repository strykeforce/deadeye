#include "capture/nv_argus_camera.h"

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include "config/capture_config.h"

using namespace deadeye;
using json = nlohmann::json;

TEST_CASE("NvArgusCamera config", "[capture]") {
  CaptureConfig config;
  REQUIRE(config.config.is_object());
}

TEST_CASE("NvArgusCamera pipeline", "[capture]") {
  CaptureConfig config{CaptureType::jetson, 640, 180, 120, json::object()};
  auto pipeline = NvArgusCameraPipeline(config);
  REQUIRE_FALSE(pipeline.find("nvarguscamerasrc") == std::string::npos);
}