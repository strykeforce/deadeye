#pragma once

#include <atomic>
#include <chrono>
#include <string>

#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "link/target_data.h"
#include "log/logger_impl.h"

namespace deadeye {
namespace logger {

class FourUp : public LoggerImpl {
 public:
  FourUp(std::string id, CaptureConfig capture_config,
         PipelineConfig pipeline_config, LogConfig log_config,
         LoggerQueue& queue, std::atomic<bool>& cancel);
  void Run() override;

 private:
  CaptureConfig capture_;
  cv::Scalar hsv_low_;
  cv::Scalar hsv_high_;
  FilterConfig filter_;

  std::chrono::high_resolution_clock::time_point begin_{
      std::chrono::high_resolution_clock::now()};
};
}  // namespace logger

}  // namespace deadeye
