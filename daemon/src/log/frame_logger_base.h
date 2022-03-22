#pragma once

#include <readerwriterqueue/readerwriterqueue.h>

#include <utility>

#include "client_logger.h"
#include "link/target_data.h"
#include "pipeline/pipeline.h"  // for Contours
#include "state.h"

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
                  FrameLoggerState& state, FrameLoggerQueue& queue,
                  std::atomic<bool>& cancel);
  virtual ~FrameLoggerBase() = default;

  inline std::string GetFrameImagePath(int frame_count) {
    return fmt::format(template_, frame_count);
  }

  void Run();

 protected:
  std::string id_;
  bool enabled_;
  std::string template_;
  int frame_count_{1};
  std::atomic<bool>& cancel_;
  FrameLoggerQueue& queue_;
  ClientLogger client_logger_;

  virtual void RunLoop() = 0;

 private:
  bool CheckMount(const FrameLogConfig& config);
  bool CheckDir(const FrameLogConfig& config);
};
}  // namespace deadeye::logger
