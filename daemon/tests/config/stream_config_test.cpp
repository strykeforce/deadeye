#include "config/stream_config.h"

#include "catch2/catch.hpp"

using namespace deadeye;

TEST_CASE("StreamConfig equality", "[pipeline]") {
  StreamConfig sc1{0, "http://example.com", StreamConfig::View::none,
                   StreamConfig::Contour::none};
  StreamConfig sc2{0, "http://example.com", StreamConfig::View::none,
                   StreamConfig::Contour::none};
  REQUIRE(sc1 == sc2);

  StreamConfig sc3{0, "foo", StreamConfig::View::none,
                   StreamConfig::Contour::none};
  REQUIRE(sc1 != sc3);
}

TEST_CASE("StreamConfig from JSON", "[pipeline]") {
  json j;
  j[StreamConfig::kSerialKey] = 2767;
  j[StreamConfig::kUrlKey] = "http://strykeforce.org";
  j[StreamConfig::kViewKey] = "original";
  j[StreamConfig::kContourKey] = "all";

  auto sc = j.get<StreamConfig>();
  REQUIRE(sc.sn == 2767);
  REQUIRE(sc.url == j[StreamConfig::kUrlKey]);
  REQUIRE(sc.view == StreamConfig::View::original);
  REQUIRE(sc.contour == StreamConfig::Contour::all);
}

TEST_CASE("StreamConfig to JSON", "[pipeline]") {
  StreamConfig sc{2767, "http://example.com", StreamConfig::View::none,
                  StreamConfig::Contour::none};
  json j = sc;
  json expected;
  expected[StreamConfig::kSerialKey] = 2767;
  expected[StreamConfig::kUrlKey] = "http://example.com";
  expected[StreamConfig::kViewKey] = "none";
  expected[StreamConfig::kContourKey] = "none";
  REQUIRE(j == expected);
}

TEST_CASE("StreamConfig New", "[pipeline]") {
  StreamConfig expected{2767, "http://example.com", StreamConfig::View::none,
                        StreamConfig::Contour::none};
  json j = expected;
  auto val = nt::Value::MakeString(j.dump());
  auto sc = StreamConfig{val};
  REQUIRE(sc == expected);
}
