#pragma once

#include <atomic>
#include <chrono>
#include <string>

#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "link/target_data.h"
#include "log/logger_impl.h"

namespace deadeye {

class FourUp : public LoggerImpl {
 public:
  FourUp(std::string id, CaptureConfig capture_config,
         PipelineConfig pipeline_config, LoggerQueue& queue,
         std::atomic<bool>& cancel);
  void log() override;

 private:
  bool CheckMount(const LogConfig& config);
  bool CheckDir(const LogConfig& config);

  std::string id_;

  static int enable_count_;
  bool enabled_;
  CaptureConfig capture_;
  cv::Scalar hsv_low_;
  cv::Scalar hsv_high_;
  FilterConfig filter_;
  std::string template_;
  LoggerQueue& queue_;
  std::atomic<bool>& cancel_;
  std::chrono::high_resolution_clock::time_point begin_{
      std::chrono::high_resolution_clock::now()};
};

}  // namespace deadeye
