#include "config/pipeline_config.hpp"
#include "config/gstreamer_config.hpp"

#include <opencv2/core/mat.hpp>
#include "catch2/catch.hpp"

using namespace deadeye;

TEST_CASE("PipelineConfig HSV bound as array", "[pipeline]") {
  std::array<int, 3> ary{1, 2, 3};
  cv::_InputArray iary{ary};
  REQUIRE(iary.isVector());
}

TEST_CASE("PipelineConfig equality", "[pipeline]") {
  PipelineConfig pc1{0, {1, 2}, {25, 24}, {250, 251}, 0.5, GStreamerConfig{}};
  PipelineConfig pc2{0, {1, 2}, {25, 24}, {250, 251}, 0.5, GStreamerConfig{}};
  REQUIRE(pc1 == pc2);

  PipelineConfig pc3{0, {0, 1}, {253, 255}, {23, 45}, 0.5, GStreamerConfig{}};
  REQUIRE(pc1 != pc3);
}

TEST_CASE("PipelineConfig to JSON", "[pipeline]") {
  PipelineConfig pc{2767, {1, 2}, {25, 24}, {250, 251}, 0.5, GStreamerConfig{}};
  json j = pc;
  json expected = R"(
{"exposure":0.5,"gstreamer":{"ch":0,"cw":0,"flip":0,"fps":0,"oh":0,"ow":0},
  "hue":[1,2],"sat":[25,24],"sn":2767,"val":[250,251]}
)"_json;

  REQUIRE(j == expected);
}

TEST_CASE("PipelineConfig New", "[pipeline]") {
  PipelineConfig expected{2767,       {1, 2}, {25, 24},
                          {250, 251}, 0.5,    GStreamerConfig{}};
  json j = expected;
  auto val = nt::Value::MakeString(j.dump());
  auto pc = PipelineConfig{val};
  REQUIRE(pc == expected);
}

TEST_CASE("PipelineConfig from JSON", "[pipeline]") {
  auto expected = R"(
{
  "sn": 2767,
  "exposure": 0.75,
  "hue": [2, 4],
  "sat": [4, 6],
  "val": [8, 10],
  "gstreamer": {
      "cw": 1,
      "ch": 2,
      "ow": 3,
      "oh": 4,
      "fps": 5,
      "flip": 6
    }
  }
)"_json;

  PipelineConfig pc = expected;

  REQUIRE(pc.sn == 2767);
  REQUIRE(pc.exposure == 0.75);
  REQUIRE(pc.hue[0] == 2);
  REQUIRE(pc.hue[1] == 4);
  REQUIRE(pc.sat[0] == 4);
  REQUIRE(pc.sat[1] == 6);
  REQUIRE(pc.val[0] == 8);
  REQUIRE(pc.val[1] == 10);
  REQUIRE(pc.gstreamer_config.capture_width == 1);
  REQUIRE(pc.gstreamer_config.capture_height == 2);
  REQUIRE(pc.gstreamer_config.output_width == 3);
  REQUIRE(pc.gstreamer_config.output_height == 4);
  REQUIRE(pc.gstreamer_config.frame_rate == 5);
  REQUIRE(pc.gstreamer_config.flip_mode == 6);
}
