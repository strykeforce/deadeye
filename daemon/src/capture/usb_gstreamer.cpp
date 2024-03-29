// Copyright (c) 2022 Stryke Force FRC 2767

#include "capture/usb_gstreamer.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <string>

using ::deadeye::UsbGStreamer;
using json = nlohmann::json;

[[maybe_unused]] UsbGStreamer::UsbGStreamer(const CaptureConfig& config) {
  auto pipeline = BuildV4L2Pipeline(config);
  is_yuv_ = false;
  cap_.open(pipeline, cv::CAP_GSTREAMER);

  spdlog::debug("V4L2: opened cap_: {}", pipeline);
}

std::string deadeye::BuildV4L2Pipeline(const CaptureConfig& config) {
  json j = config.config;
  if (!j.is_object()) j = json::object();

  auto device = j.value("device", "/dev/video0");

  const char* src_fmt =
      "v4l2src "
      "device={}";

  auto src = fmt::format(src_fmt, device);

  auto cap_width = j.value("captureWidth", 640);
  auto cap_height = j.value("captureHeight", 480);
  auto format = j.value("format", "YUY2");

  const char* cap_format_fmt =
      "video/x-raw,"
      "format={},"
      "width=(int){},"
      "height=(int){},"
      "framerate=(fraction){}/1";

  auto cap_format = fmt::format(cap_format_fmt, format, cap_width, cap_height,
                                config.frame_rate);

  const char* output = "videoconvert ! video/x-raw, format=(string)BGR";

  auto pipeline = src + " ! " + cap_format + " ! " + output + " !  appsink";
  return pipeline;
}
