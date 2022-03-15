#pragma once

#include <atomic>
#include <chrono>
#include <string>

#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "link/target_data.h"
#include "log/frame_logger_base.h"

namespace deadeye::logger {

class FourUp : public FrameLoggerBase {
 public:
  FourUp(const int inum, const CaptureConfig& capture_config,
         const PipelineConfig& pipeline_config,
         const FrameLogConfig& log_config, FrameLoggerQueue& queue,
         std::atomic<bool>& cancel);
  void Run() override;

 private:
  cv::Mat InfoPane(const FrameLogEntry& entry, const Contours& contours,
                   int seq, int elapsed);

  std::string capture_;
  int width_;
  int height_;
  cv::Scalar hsv_low_;
  cv::Scalar hsv_high_;
  FilterConfig filter_;

  std::chrono::high_resolution_clock::time_point begin_;
};
}  // namespace deadeye::logger