#include "pipeline.hpp"

#include <opencv2/core/mat.hpp>
#include "catch2/catch.hpp"

using namespace deadeye;

TEST_CASE("PipelineConfig HSV bound as array", "[pipeline]") {
  std::array<int, 3> ary{1, 2, 3};
  cv::_InputArray iary{ary};
  REQUIRE(iary.isVector());
}

TEST_CASE("PipelineConfig equality", "[pipeline]") {
  PipelineConfig pc1{0, {1, 2}, {25, 24}, {250, 251}, 0.5};
  PipelineConfig pc2{0, {1, 2}, {25, 24}, {250, 251}, 0.5};
  REQUIRE(pc1 == pc2);

  PipelineConfig pc3{0, {0, 1}, {253, 255}, {23, 45}, 0.5};
  REQUIRE(pc1 != pc3);
}

TEST_CASE("PipelineConfig from JSON", "[pipeline]") {
  json j;
  j[PipelineConfig::kSerialKey] = 2767;
  j[PipelineConfig::kHsvHueKey] = {1, 2};
  j[PipelineConfig::kHsvSatKey] = {100, 200};
  j[PipelineConfig::kHsvValKey] = {1, 250};
  j[PipelineConfig::kExposureKey] = 0.33;

  auto pc = j.get<PipelineConfig>();
  REQUIRE(pc.sn == 2767);
  REQUIRE(pc.hue == j[PipelineConfig::kHsvHueKey]);
  REQUIRE(pc.sat == j[PipelineConfig::kHsvSatKey]);
  REQUIRE(pc.val == j[PipelineConfig::kHsvValKey]);
  REQUIRE(pc.exposure == 0.33);
}

TEST_CASE("PipelineConfig to JSON", "[pipeline]") {
  PipelineConfig pc{2767, {1, 2}, {25, 24}, {250, 251}, 0.5};
  json j = pc;
  json expected;
  expected[PipelineConfig::kSerialKey] = 2767;
  expected[PipelineConfig::kHsvHueKey] = {1, 2};
  expected[PipelineConfig::kHsvSatKey] = {25, 24};
  expected[PipelineConfig::kHsvValKey] = {250, 251};
  expected[PipelineConfig::kExposureKey] = 0.5;

  REQUIRE(j == expected);
}

TEST_CASE("PipelineConfig New", "[pipeline]") {
  PipelineConfig expected{2767, {1, 2}, {25, 24}, {250, 251}, 0.5};
  json j = expected;
  auto val = nt::Value::MakeString(j.dump());
  auto pc = PipelineConfig::New(val);
  REQUIRE(pc == expected);
}
