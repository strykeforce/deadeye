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

TEST_CASE("Serialize State", "[state]") {
  std::vector<PipelineState> pipelines{PipelineState{FrameLoggerState{27}},
                                       PipelineState{FrameLoggerState{67}}};
  json j = State{pipelines};
  json exp = R"(
{
  "pipelines": [
    {"frame": {"sequence": 27} },
    {"frame": {"sequence": 67} }
  ]
}
)"_json;

  REQUIRE(j == exp);
}

TEST_CASE("Deserialize State", "[state]") {
  State s = json::parse(R"(
{
  "pipelines": [
    {"frame": {"sequence": 27} },
    {"frame": {"sequence": 67} }
  ]
}
)");
  REQUIRE(s.pipelines[0].frame_logger.sequence == 27);
  REQUIRE(s.pipelines[1].frame_logger.sequence == 67);
}

TEST_CASE("Deserialize State from input stream", "[state]") {
  std::istringstream buf{
      R"(
{
  "pipelines": [
    {"frame": {"sequence": 27} },
    {"frame": {"sequence": 67} }
  ]
}
)"};

  State s = State::Load(buf);
  REQUIRE(s.pipelines[0].frame_logger.sequence == 27);
  REQUIRE(s.pipelines[1].frame_logger.sequence == 67);
}

TEST_CASE("Serialize State to file stream", "[state]") {
  std::vector<PipelineState> pipelines{PipelineState{FrameLoggerState{27}},
                                       PipelineState{FrameLoggerState{67}}};
  State s = State{pipelines};
  std::ostringstream oss{};
  s.Store(oss);
  REQUIRE(oss.str() == R"({"pipelines":[{"frame":{"sequence":27}},{"frame":{"sequence":67}}]})");
}
