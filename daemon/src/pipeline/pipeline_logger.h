#pragma once

#include <readerwriterqueue.h>
#include <atomic>
#include <opencv2/core/mat.hpp>
#include <string>

#include "config/log_config.h"
#include "link/target_data.h"
#include "pipeline/pipeline.h"  // for Contours

namespace deadeye {

struct PipelineLogEntry {
  PipelineLogEntry() = default;
  PipelineLogEntry(cv::Mat const frame, cv::Mat const mask, Contours contours,
                   Contours filtered_contours, TargetDataPtr target);

  cv::Mat frame;
  cv::Mat mask;
  Contours contours;
  Contours filtered_contours;
  TargetDataPtr target;
};

using PipelineLoggerQueue =
    moodycamel::BlockingReaderWriterQueue<PipelineLogEntry>;

class PipelineLogger {
 public:
  PipelineLogger(std::string id, LogConfig config, PipelineLoggerQueue& queue,
                 std::atomic<bool>& cancel);
  void operator()();

 private:
  std::string id_;
  bool enabled_;
  std::string template_;
  PipelineLoggerQueue& queue_;
  std::atomic<bool>& cancel_;
};

}  // namespace deadeye
