#pragma once

#include <opencv2/core/types.hpp>

#include "config/capture_config.h"

namespace deadeye {

class Capture {
 public:
  virtual ~Capture() {}
  virtual bool Grab(cv::Mat& frame) = 0;
};
}  // namespace deadeye