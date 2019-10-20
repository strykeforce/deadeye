#include "config/gstreamer_config.hpp"
#include "catch2/catch.hpp"

using namespace deadeye;

TEST_CASE("GStreamerConfig equality", "[gstreamer]") {
  GStreamerConfig gsc1{0, 1, 2, 3, 4, 5};
  GStreamerConfig gsc2{0, 1, 2, 3, 4, 5};
  REQUIRE(gsc1 == gsc2);

  GStreamerConfig gsc3{1, 2, 3, 4, 5, 6};
  REQUIRE(gsc1 != gsc3);
}

TEST_CASE("GetJetsonCSI", "[gstreamer]") {
  GStreamerConfig gsc{1280, 720, 320, 180, 120, 0};
  std::string actual = gsc.GetJetsonCSI();
  std::string expected =
      R"(nvarguscamerasrc awblock=true aelock=true wbmode=0 ispdigitalgainrange="1 1" gainrange="1 2" exposuretimerange="13000 683709000" ! video/x-raw(memory:NVMM), width=(int)1280, height=(int)720, format=(string)NV12, framerate=(fraction)120/1 ! nvvidconv flip-method=0 ! video/x-raw, width=(int)320, height=(int)180, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink)";
  REQUIRE(actual == expected);
}
