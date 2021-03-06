#pragma once

#include <opencv2/core/mat.hpp>

#include "capture/capture.h"
#include "config/capture_config.h"

namespace deadeye {

class ImageFile : public Capture {
 public:
  ImageFile(const CaptureConfig& config);
  virtual ~ImageFile() {}

  virtual bool Grab(cv::Mat& frame) override;

 private:
  cv::Mat src_;
  cv::Size size_;
  int fps_;

  void LoadInvalidSource(const std::string& path);
  void LoadSource(const std::string& path);
};
}  // namespace deadeye