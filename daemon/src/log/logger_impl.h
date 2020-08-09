#pragma once

#include <readerwriterqueue/readerwriterqueue.h>

#include "link/target_data.h"
#include "pipeline/pipeline.h"  // for Contours

namespace deadeye {

namespace logger {

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

using LoggerQueue =
    moodycamel::BlockingReaderWriterQueue<deadeye::logger::LogEntry>;

class LoggerImpl {
 public:
  LoggerImpl(std::string id, LogConfig config, LoggerQueue& queue,
             std::atomic<bool>& cancel);
  virtual ~LoggerImpl() {}

  virtual void Run() = 0;

 protected:
  std::string id_;
  bool enabled_;
  std::string template_;
  std::atomic<bool>& cancel_;
  LoggerQueue& queue_;

 private:
  bool CheckMount(const LogConfig& config);
  bool CheckDir(const LogConfig& config);
  static int enable_count_;
};
}  // namespace logger

}  // namespace deadeye
