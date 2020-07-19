#pragma once

#include <readerwriterqueue/readerwriterqueue.h>

#include <atomic>
#include <chrono>
#include <opencv2/core/mat.hpp>
#include <string>

#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "link/target_data.h"
#include "pipeline/pipeline.h"  // for Contours

namespace deadeye {

struct PipelineLogEntry {
  PipelineLogEntry() = default;
  PipelineLogEntry(cv::Mat const frame, Contours filtered_contours,
                   TargetDataPtr target);

  cv::Mat frame;
  Contours filtered_contours;
  TargetDataPtr target;
};

using PipelineLoggerQueue =
    moodycamel::BlockingReaderWriterQueue<PipelineLogEntry>;

class PipelineLogger {
 public:
  PipelineLogger(std::string id, CaptureConfig capture_config,
                 PipelineConfig pipeline_config, PipelineLoggerQueue& queue,
                 std::atomic<bool>& cancel);
  void operator()();

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
  PipelineLoggerQueue& queue_;
  std::atomic<bool>& cancel_;
  std::chrono::high_resolution_clock::time_point begin_{
      std::chrono::high_resolution_clock::now()};
};

}  // namespace deadeye
