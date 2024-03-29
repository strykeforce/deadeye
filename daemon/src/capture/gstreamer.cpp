// Copyright (c) 2022 Stryke Force FRC 2767

#include "capture/gstreamer.h"

#include <spdlog/spdlog.h>

#include <opencv2/imgproc.hpp>

using ::deadeye::Gstreamer;

Gstreamer::~Gstreamer() {
  cap_.release();
  spdlog::debug("GStreamer: released cap_");
}

bool Gstreamer::Grab(cv::Mat& frame) {
  bool is_ok = cap_.read(frame);
  if (is_ok && is_yuv_) cv::cvtColor(frame, frame, cv::COLOR_YUV2BGR_UYVY);
  return is_ok;
}
