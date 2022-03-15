#include "log/full_frame.h"

#include <fmt/core.h>

#include <opencv2/imgcodecs.hpp>

using namespace deadeye::logger;

FullFrame::FullFrame(const int inum, const CaptureConfig& capture_config,
                     const FrameLogConfig& log_config, FrameLoggerQueue& queue,
                     std::atomic<bool>& cancel)
    : FrameLoggerBase(inum, log_config, queue, cancel) {}

void FullFrame::Run() {
  int seq = 1;
  FrameLogEntry entry;
  if (enabled_)
    client_logger.Info(
        fmt::format("FullFrame<{}>: logging to " + template_, id_, id_, "nnn"));
  else
    client_logger.Warn(fmt::format("FullFrame<{}>: logging disabled", id_));

  while (!cancel_.load()) {
    if (!queue_.wait_dequeue_timed(entry, std::chrono::milliseconds(100))) {
      continue;
    }
    if (!enabled_) continue;  // throw away if logged by upstream while disabled

    auto path = fmt::format(template_, id_, seq);

    try {
      cv::imwrite(path, entry.frame);
    } catch (const cv::Exception& ex) {
      spdlog::error("Capture<{}>: logger write exception: {}", id_, ex.what());
    }
    spdlog::trace("Capture<{}>: logger wrote image to {}", id_, path);

    if (queue_.size_approx() > 0)
      spdlog::warn("Capture<{}>: logger queue filling: {}", id_,
                   queue_.size_approx());

    seq++;
  }
}