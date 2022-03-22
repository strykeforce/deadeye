#include <networktables/NetworkTableInstance.h>

#include <opencv2/core/mat.hpp>

#include "catch2/catch.hpp"
#include "config.h"
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
  json j = PipelineState{0, FrameLoggerState{2767}};
  json exp = R"({"frame": {"sequence": 2767} })"_json;
  REQUIRE(j == exp);
}

TEST_CASE("Deserialize PipelineState", "[state]") {
  PipelineState pls = json::parse(R"({"frame": {"sequence": 2767} })");
  REQUIRE(pls.frame_logger.sequence == 2767);
}

TEST_CASE("Load & store PipelineState", "[state]") {
  nt::NetworkTableInstance nti = nt::NetworkTableInstance::GetDefault();
  int inum = 1;
  auto entry = nti.GetEntry(StateEntryPath(inum));
  entry.Delete();

  SECTION("default pipeline state before first store") {
    PipelineState state = PipelineState::Load(inum);
    REQUIRE(state.frame_logger.sequence == 1);
  }

  SECTION("load stored state") {
    entry.SetString(R"({"frame": {"sequence": 2767} })");
    PipelineState state = PipelineState::Load(inum);
    REQUIRE(state.frame_logger.sequence == 2767);
  }

  SECTION("store state") {
    PipelineState state{inum, FrameLoggerState{1234}};
    state.Store();
    json j = json::parse(entry.GetString("{}"));
    json exp = R"({"frame": {"sequence": 1234} })"_json;
    REQUIRE(j == exp);
  }

  SECTION("load, update & store state") {
    entry.SetString(R"({"frame": {"sequence": 2767} })");
    PipelineState state = PipelineState::Load(inum);
    REQUIRE(state.frame_logger.sequence == 2767);
    state.frame_logger.sequence++;
    state.Store();
    state = PipelineState::Load(inum);
    REQUIRE(state.frame_logger.sequence == 2768);
  }
}
