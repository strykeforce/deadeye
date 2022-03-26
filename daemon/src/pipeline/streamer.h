// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <cscore.h>
#include <cscore_cv.h>

#include <opencv2/core/mat.hpp>

#include "config/capture_config.h"
#include "config/stream_config.h"
#include "link/target_data.h"

namespace deadeye {

class Pipeline;

class Streamer {
 public:
  Streamer(const Pipeline* pipeline, cv::Size size);
  void Configure(const StreamConfig& config);
  void Process(const cv::Mat& frame, const TargetData* target);

 private:
  const Pipeline* pipeline_;
  cv::Mat output_;
  StreamConfig config_;
  int border_;
  bool resize_;
  cs::MjpegServer server_;
  cs::CvSource source_;
};
}  // namespace deadeye
