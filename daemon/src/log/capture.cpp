#include "log/capture.h"

#include <fmt/core.h>

#include <opencv2/imgcodecs.hpp>
#include <utility>

using namespace deadeye::logger;

Capture::Capture(std::string id, const CaptureConfig& capture_config,
                 const LogConfig& log_config, LoggerQueue& queue,
                 std::atomic<bool>& cancel)
    : LoggerImpl(std::move(id), log_config, queue, cancel) {}

void Capture::Run() {
  int seq = 1;
  LogEntry entry;
  if (enabled_)
    spdlog::info("Capture<{}>: logging to {}", id_,
                 fmt::format(template_, id_, "nnn"));
  else
    spdlog::warn("Capture<{}>: logging disabled", id_);

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