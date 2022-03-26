// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <cscore_cv.h>

#include "capture/capture.h"

namespace deadeye {
class UsbCsCore : public Capture {
 public:
  explicit UsbCsCore(const CaptureConfig& config);
  ~UsbCsCore() override = default;
  bool Grab(cv::Mat& frame) override;

 private:
  cs::CvSink cv_sink_{"cvsink"};
};
}  // namespace deadeye
