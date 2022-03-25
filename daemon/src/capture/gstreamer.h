// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <opencv2/core/types.hpp>
#include <opencv2/videoio.hpp>

#include "capture/capture.h"

namespace deadeye {

class Gstreamer : public Capture {
 public:
  ~Gstreamer() override;

  bool Grab(cv::Mat& frame) override;

 protected:
  cv::VideoCapture cap_;
  bool is_yuv_{false};
};
}  // namespace deadeye
