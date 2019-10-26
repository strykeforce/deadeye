#include "config/gstreamer_config.h"
// #include <iostream>
#include "catch2/catch.hpp"

using namespace deadeye;
using Type = GStreamerConfig::Type;

TEST_CASE("GStreamerConfig equality", "[gstreamer]") {
  GStreamerConfig gsc1{Type::test, 0, 1, 2, 3, 4, 5, 0.25};
  GStreamerConfig gsc2{Type::test, 0, 1, 2, 3, 4, 5, 0.25};
  REQUIRE(gsc1 == gsc2);

  GStreamerConfig gsc3{Type::test, 1, 2, 3, 4, 5, 6, 0.5};
  REQUIRE(gsc1 != gsc3);
}

TEST_CASE("Pipeline", "[gstreamer]") {
  GStreamerConfig gsc{Type::test, 1280, 720, 320, 180, 120, 0, 0.5};
  std::string expected =
      R"(nvarguscamerasrc aelock=true awblock=false wbmode=0 ispdigitalgainrange="1 1" gainrange="1 1" exposuretimerange="4160166 4160166" ! video/x-raw(memory:NVMM), width=(int)1280, height=(int)720, format=(string)NV12, framerate=(fraction)120/1 ! nvvidconv flip-method=0 ! video/x-raw, width=(int)320, height=(int)180, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink)";
  std::string actual = gsc.Pipeline();
  // std::cerr << "E:" << expected << std::endl;
  // std::cerr << "A:" << actual << std::endl;
  REQUIRE(actual == expected);
}
