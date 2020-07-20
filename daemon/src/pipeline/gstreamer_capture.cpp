#include "pipeline/gstreamer_capture.h"

#include <spdlog/spdlog.h>

#include <opencv2/imgproc.hpp>

using namespace deadeye;

GstreamerCapture::GstreamerCapture(CaptureConfig config)
    : config_{config}, is_yuv_{config.type == CaptureConfig::Type::jetson} {
  auto pipeline = config_.Pipeline();
  cap_.open(pipeline, cv::CAP_GSTREAMER);
  // TODO: some error checks and logging
}

GstreamerCapture::~GstreamerCapture() {
  cap_.release();
  spdlog::debug("released cap_");
}

bool GstreamerCapture::Grab(cv::Mat& frame) {
  bool is_ok = cap_.read(frame);
  if (is_ok && is_yuv_) cv::cvtColor(frame, frame, cv::COLOR_YUV2BGR_I420);
  return is_ok;
};
