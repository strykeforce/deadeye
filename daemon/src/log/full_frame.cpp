#include "log/full_frame.h"

#include <fmt/core.h>

#include <opencv2/imgcodecs.hpp>

using namespace deadeye::logger;

FullFrame::FullFrame(const int inum, const CaptureConfig& capture_config,
                     const FrameLogConfig& log_config, FrameLoggerState& state,
                     FrameLoggerQueue& queue, std::atomic<bool>& cancel)
    : FrameLoggerBase(inum, log_config, state, queue, cancel) {}

void FullFrame::RunLoop() {
  frame_count_ = 1;
  FrameLogEntry entry;

  while (!cancel_.load()) {
    if (!queue_.wait_dequeue_timed(entry, std::chrono::milliseconds(100))) {
      continue;
    }
    if (!enabled_) continue;  // throw away if logged by upstream while disabled

    std::string path = GetFrameImagePath(frame_count_);
    try {
      cv::imwrite(path, entry.frame);
    } catch (const cv::Exception& ex) {
      spdlog::error("Capture<{}>: logger write exception: {}", id_, ex.what());
    }
    spdlog::trace("Capture<{}>: logger wrote image to {}", id_, path);

    if (queue_.size_approx() > 0)
      spdlog::warn("Capture<{}>: logger queue filling: {}", id_,
                   queue_.size_approx());

    frame_count_++;
  }
}