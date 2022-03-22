#include <opencv2/core/mat.hpp>

#include "catch2/catch.hpp"
#include "config/link_config.h"
#include "state.h"

using namespace deadeye;
using json = nlohmann::json;

TEST_CASE("Serialize FrameLoggerState", "[state]") {
  json j = FrameLoggerState{2767};
  json exp = R"({"sequence": 2767})"_json;
  REQUIRE(j == exp);
}

TEST_CASE("Deserialize FrameLoggerState", "[state]") {
  FrameLoggerState fls = json::parse(R"({"sequence": 2767})");
  REQUIRE(fls.sequence == 2767);
}

TEST_CASE("Serialize PipelineState", "[state]") {
  json j = PipelineState{FrameLoggerState{2767}};
  json exp = R"({"frame": {"sequence": 2767} })"_json;
  REQUIRE(j == exp);
}

TEST_CASE("Deserialize PipelineState", "[state]") {
  PipelineState pls = json::parse(R"({"frame": {"sequence": 2767} })");
  REQUIRE(pls.frame_logger.sequence == 2767);
}
