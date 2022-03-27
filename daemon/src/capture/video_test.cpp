// Copyright (c) 2022 Stryke Force FRC 2767

#include "capture/video_test.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

using ::deadeye::VideoTest;
using json = nlohmann::json;

VideoTest::VideoTest(const CaptureConfig& config) {
  json j = config.config;
  if (!j.is_object()) j = json::object();

  auto pattern = j.value("pattern", "smpte");
  auto foreground_color = j.value("foregroundColor", "ffffff");
  auto background_color = j.value("backgroundColor", "000000");
  auto speed = j.value("speed", 0);

  std::string_view flip = config.width == 720 && config.height == 1280
                              ? "videoflip method=clockwise !"
                              : "";

  auto pipeline = fmt::format(
      "videotestsrc pattern={} foreground-color=0x{} background-color=0x{} "
      "horizontal-speed={} ! video/x-raw, width={}, height={}, "
      "framerate={}/1 ! {}  videoconvert ! video/x-raw, format=(string)BGR ! "
      "appsink",
      pattern, foreground_color, background_color, speed, config.width,
      config.height, config.frame_rate, flip);

  cap_.open(pipeline, cv::CAP_GSTREAMER);
  spdlog::debug("VideoTest: opened cap_: {}", pipeline);
}
