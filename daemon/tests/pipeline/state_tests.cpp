#include "config/link_config.h"

#include <opencv2/core/mat.hpp>

#include "catch2/catch.hpp"
#include "pipeline/state.h"

using namespace deadeye;
using json = nlohmann::json;

TEST_CASE("Load State", "[state]") {
  State state = State::Load(DEADEYE_TEST_DATA "/state.json");
  REQUIRE(state.initial_frame_sequence == 2767);
}
