// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <memory>
#include <opencv2/core/types.hpp>

#include "config/capture_config.h"

namespace deadeye {

class Capture {
 public:
  virtual ~Capture() = default;
  virtual bool Grab(cv::Mat& frame) = 0;
};
}  // namespace deadeye
