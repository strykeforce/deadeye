#pragma once

#include <cscore_cv.h>
#include "capture/capture.h"

namespace deadeye {
class UsbCsCore : public Capture {
 public:
  UsbCsCore(const CaptureConfig& config);
  virtual ~UsbCsCore() {}
  virtual bool Grab(cv::Mat& frame) override;

 private:
  cs::CvSink cv_sink_{"cvsink"};
};
}  // namespace deadeye
