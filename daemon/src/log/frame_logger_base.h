#pragma once

#include <readerwriterqueue/readerwriterqueue.h>

#include <utility>

#include "link/target_data.h"
#include "pipeline/pipeline.h"  // for Contours
#include "client_logger.h"

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

using FrameLoggerQueue =
    moodycamel::BlockingReaderWriterQueue<deadeye::logger::FrameLogEntry>;

class FrameLoggerBase {
 public:
  FrameLoggerBase(int inum, const FrameLogConfig& config,
                  FrameLoggerQueue& queue,
                  std::atomic<bool>& cancel);
  virtual ~FrameLoggerBase() = default;

  virtual void Run() = 0;

 protected:
  std::string id_;
  bool enabled_;
  std::string template_;
  std::atomic<bool>& cancel_;
  FrameLoggerQueue& queue_;
  ClientLogger client_logger;

 private:
  bool CheckMount(const FrameLogConfig& config);
  bool CheckDir(const FrameLogConfig& config);
  static int enable_count_;
};
}  // namespace deadeye::logger
