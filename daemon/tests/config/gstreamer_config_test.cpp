// #include <iostream>
#include "catch2/catch.hpp"
#include "config/capture_config.h"

using namespace deadeye;
using Type = CaptureConfig::Type;

TEST_CASE("CaptureConfig equality", "[gstreamer]") {
  CaptureConfig cc1{Type::test, 0, 1, 2, 3, 4, 5, 0.25};
  CaptureConfig cc{Type::test, 0, 1, 2, 3, 4, 5, 0.25};
  REQUIRE(cc1 == cc);

  CaptureConfig gsc3{Type::test, 1, 2, 3, 4, 5, 6, 0.5};
  REQUIRE(cc1 != gsc3);
}

TEST_CASE("Pipeline", "[gstreamer]") {
  CaptureConfig cc{Type::jetson, 1280, 720, 320, 180, 120, 0, 0.5};
  std::string expected =
      R"(nvarguscamerasrc aelock=true awblock=false wbmode=0 ispdigitalgainrange="1 1" gainrange="1 1" exposuretimerange="4160166 4160166" ! video/x-raw(memory:NVMM), width=(int)1280, height=(int)720, format=(string)NV12, framerate=(fraction)120/1 ! nvvidconv flip-method=0 ! video/x-raw, format=I420, width=(int)320, height=(int)180 ! appsink)";
  std::string actual = cc.Pipeline();
  // std::cerr << "E:" << expected << std::endl;
  // std::cerr << "A:" << actual << std::endl;
  REQUIRE(actual == expected);
}
