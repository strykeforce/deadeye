#pragma once

#include <spdlog/spdlog.h>

#include <atomic>
#include <future>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <utility>

#include "log/capture.h"
#include "log/four_up.h"
#include "log/frame_log_type.h"
#include "log/frame_logger_impl.h"

namespace deadeye {

class FrameLogger {
 public:
  FrameLogger() = default;
  FrameLogger(const std::string& id, const CaptureConfig& capture_config,
              const PipelineConfig& pipeline_config,
              const FrameLogConfig& log_config) {
    switch (log_config.type) {
      case FrameLogType::capture:
        logger_ = std::make_unique<logger::Capture>(
            id, capture_config, log_config, queue_, cancel_);
        break;

      case FrameLogType::four_up:
        logger_ = std::make_unique<logger::FourUp>(
            id, capture_config, pipeline_config, log_config, queue_, cancel_);

      default:
        break;
    }
  };
  void Run() {
    future_ = std::async(std::launch::async, &logger::FrameLoggerImpl::Run,
                         logger_.get());
    spdlog::debug("Logger::Run starting async logging task");
  }

  void Log(cv::Mat const& frame, Contours filtered_contours,
           std::unique_ptr<TargetData> target) {
    queue_.enqueue(logger::FrameLogEntry{
        frame.clone(), std::move(filtered_contours), std::move(target)});
  }

  void Stop() {
    cancel_ = true;
    future_.wait();
    spdlog::debug("Logger::Stop stopped async logging task");
  }

 private:
  std::atomic<bool> cancel_{false};
  std::unique_ptr<logger::FrameLoggerImpl> logger_;
  std::future<void> future_;
  logger::LoggerQueue queue_;
};

}  // namespace deadeye
