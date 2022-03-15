#pragma once

#include <readerwriterqueue/readerwriterqueue.h>

#include <utility>

#include "link/target_data.h"
#include "pipeline/pipeline.h"  // for Contours

namespace deadeye::logger {

struct FrameLogEntry {
  FrameLogEntry() = default;

  FrameLogEntry(cv::Mat frame, Contours filtered_contours,
                std::unique_ptr<TargetData> target)
      : frame{std::move(frame)},
        filtered_contours{std::move(filtered_contours)},
        target{std::move(target)} {}

  cv::Mat frame;
  Contours filtered_contours;
  std::unique_ptr<TargetData> target;
};

using LoggerQueue =
    moodycamel::BlockingReaderWriterQueue<deadeye::logger::FrameLogEntry>;

class FrameLoggerImpl {
 public:
  FrameLoggerImpl(std::string id, const FrameLogConfig& config,
                  LoggerQueue& queue, std::atomic<bool>& cancel);
  virtual ~FrameLoggerImpl() = default;

  virtual void Run() = 0;

 protected:
  std::string id_;
  bool enabled_;
  std::string template_;
  std::atomic<bool>& cancel_;
  LoggerQueue& queue_;

 private:
  bool CheckMount(const FrameLogConfig& config);
  bool CheckDir(const FrameLogConfig& config);
  static int enable_count_;
};
}  // namespace deadeye::logger
