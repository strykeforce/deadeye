// Copyright (c) 2022 Stryke Force FRC 2767

#include "config/pipeline_config.h"

#include <opencv2/core/mat.hpp>

#include "catch2/catch.hpp"
#include "config/capture_config.h"

using ::deadeye::FilterConfig;
using ::deadeye::FrameLogConfig;
using ::deadeye::FrameLogType;
using ::deadeye::PipelineConfig;
using json = nlohmann::json;

TEST_CASE("PipelineConfig HSV bound as array", "[pipeline]") {
  std::array<int, 3> ary{1, 2, 3};
  cv::_InputArray iary{ary};
  REQUIRE(iary.isVector());
}

TEST_CASE("PipelineConfig equality", "[pipeline]") {
  PipelineConfig pc1{0,          {1, 2},         {25, 24},
                     {250, 251}, FilterConfig(), FrameLogConfig()};
  PipelineConfig pc2{0,          {1, 2},         {25, 24},
                     {250, 251}, FilterConfig(), FrameLogConfig()};
  REQUIRE(pc1 == pc2);

  PipelineConfig pc3{0,        {0, 1},         {253, 255},
                     {23, 45}, FilterConfig(), FrameLogConfig()};
  REQUIRE(pc1 != pc3);
}

TEST_CASE("PipelineConfig to JSON", "[pipeline]") {
  PipelineConfig pc{2767,       {1, 2},         {25, 24},
                    {250, 251}, FilterConfig(), FrameLogConfig()};
  json j = pc;
  json expected = R"(
{"hue":[1,2],"sat":[25,24],"sn":2767,"val":[250,251],
"log":{"fps":0,"path":"/var/opt/deadeye","mount":true,"type":"capture"},
"filter":{"area":[0.0,1.0],"aspect":[0.0,20.0],"solidity":[0.0,1.0]},
"config":{}}
)"_json;

  REQUIRE(j == expected);
}

TEST_CASE("PipelineConfig New", "[pipeline]") {
  PipelineConfig expected{2767,       {1, 2},         {25, 24},
                          {250, 251}, FilterConfig(), FrameLogConfig()};
  json j = expected;
  auto val = nt::Value::MakeString(j.dump());
  auto pc = PipelineConfig{val};
  REQUIRE(pc == expected);
}

TEST_CASE("PipelineConfig from JSON", "[pipeline]") {
  SECTION("without optional config") {
    auto expected = R"(
{
  "sn": 2767,
  "hue": [2, 4],
  "sat": [4, 6],
  "val": [8, 10],
  "log": {"fps":1, "path":"/foo"},
  "filter": {"area":[0.0,1.0], "aspect":[2.0,3.0], "solidity":[4.0,5.0]}
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
    REQUIRE(pc.log == FrameLogConfig(FrameLogType::four_up, "/foo", 1, true));
  }

  SECTION("with optional config") {
    auto expected = R"(
{
  "sn": 2767,
  "hue": [2, 4],
  "sat": [4, 6],
  "val": [8, 10],
  "log": {"fps":1, "path":"/foo"},
  "filter": {"area":[0.0,1.0], "aspect":[2.0,3.0], "solidity":[4.0,5.0]},
  "config": {"a": 2767, "b": "Stryke Force"}
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
    REQUIRE(pc.log == FrameLogConfig(FrameLogType::four_up, "/foo", 1, true));

    json j = pc.config;
    REQUIRE(j.value("a", 0) == 2767);
    REQUIRE(j.value("b", "not this") == "Stryke Force");
  }
}

TEST_CASE("LogConfig has sane defaults", "[pipeline]") {
  auto lc = FrameLogConfig();
  REQUIRE(lc.path == FrameLogConfig::kDefaultPath);
  REQUIRE(lc.fps == 0);
  REQUIRE(lc.mount);
}
