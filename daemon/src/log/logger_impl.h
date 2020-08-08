#pragma once

#include <readerwriterqueue/readerwriterqueue.h>

#include "link/target_data.h"
#include "pipeline/pipeline.h"  // for Contours

namespace deadeye {

struct LogEntry {
  LogEntry() = default;

  LogEntry(cv::Mat const frame, Contours filtered_contours,
           std::unique_ptr<TargetData> target)
      : frame{frame},
        filtered_contours{filtered_contours},
        target{std::move(target)} {}

  cv::Mat frame;
  Contours filtered_contours;
  std::unique_ptr<TargetData> target;
};

using LoggerQueue = moodycamel::BlockingReaderWriterQueue<deadeye::LogEntry>;

class LoggerImpl {
 public:
  virtual ~LoggerImpl() {}

  virtual void log() = 0;
};

}  // namespace deadeye
