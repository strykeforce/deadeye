#include "config/pipeline_config.h"
#include "config/capture_config.h"

#include <opencv2/core/mat.hpp>
#include "catch2/catch.hpp"

using namespace deadeye;

TEST_CASE("PipelineConfig HSV bound as array", "[pipeline]") {
  std::array<int, 3> ary{1, 2, 3};
  cv::_InputArray iary{ary};
  REQUIRE(iary.isVector());
}

TEST_CASE("PipelineConfig equality", "[pipeline]") {
  PipelineConfig pc1{0,          {1, 2},         {25, 24},
                     {250, 251}, FilterConfig(), LogConfig()};
  PipelineConfig pc2{0,          {1, 2},         {25, 24},
                     {250, 251}, FilterConfig(), LogConfig()};
  REQUIRE(pc1 == pc2);

  PipelineConfig pc3{0,        {0, 1},         {253, 255},
                     {23, 45}, FilterConfig(), LogConfig()};
  REQUIRE(pc1 != pc3);
}

TEST_CASE("PipelineConfig to JSON", "[pipeline]") {
  PipelineConfig pc{2767,       {1, 2},         {25, 24},
                    {250, 251}, FilterConfig(), LogConfig()};
  json j = pc;
  json expected = R"(
{"hue":[1,2],"sat":[25,24],"sn":2767,"val":[250,251],
"log":{"enabled":false,"path":"/var/opt/deadeye","mount":true},
"filter":{"area":[0.0,100.0],"aspect":[0.0,100.0],"fullness":[0.0,100.0]}}
)"_json;

  REQUIRE(j == expected);
}

TEST_CASE("PipelineConfig New", "[pipeline]") {
  PipelineConfig expected{2767,       {1, 2},         {25, 24},
                          {250, 251}, FilterConfig(), LogConfig()};
  json j = expected;
  auto val = nt::Value::MakeString(j.dump());
  auto pc = PipelineConfig{val};
  REQUIRE(pc == expected);
}

TEST_CASE("PipelineConfig from JSON", "[pipeline]") {
  auto expected = R"(
{
  "sn": 2767,
  "hue": [2, 4],
  "sat": [4, 6],
  "val": [8, 10],
  "log": {"enabled":true, "path":"/foo"},
  "filter": {"area":[0.0,1.0], "aspect":[2.0,3.0], "fullness":[4.0,5.0]}
  }
)"_json;

  PipelineConfig pc = expected;

  REQUIRE(pc.sn == 2767);
  REQUIRE(pc.hue[0] == 2);
  REQUIRE(pc.hue[1] == 4);
  REQUIRE(pc.sat[0] == 4);
  REQUIRE(pc.sat[1] == 6);
  REQUIRE(pc.val[0] == 8);
  REQUIRE(pc.val[1] == 10);
  REQUIRE(pc.filter == FilterConfig({0.0, 1.0}, {4.0, 5.0}, {2.0, 3.0}));
  REQUIRE(pc.log == LogConfig("/foo", 1, true));
}

TEST_CASE("LogConfig has sane defaults", "[pipeline]") {
  auto lc = LogConfig();
  REQUIRE(lc.path == LogConfig::kDefaultPath);
  REQUIRE(lc.fps == 1);
  REQUIRE(lc.mount);
}
