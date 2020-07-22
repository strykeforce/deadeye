#pragma once

#include <opencv2/core/types.hpp>
#include <opencv2/videoio.hpp>

#include "pipeline/capture.h"

namespace deadeye {

class GstreamerCapture : public Capture {
 public:
  GstreamerCapture(CaptureConfig config);
  virtual ~GstreamerCapture();

  virtual bool Grab(cv::Mat& frame) override;

 private:
  CaptureConfig config_;
  cv::VideoCapture cap_;
  bool is_yuv_;
};
}  // namespace deadeye