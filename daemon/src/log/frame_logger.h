#pragma once

#include <spdlog/spdlog.h>

#include <atomic>
#include <future>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <utility>

#include "log/four_up.h"
#include "log/frame_log_type.h"
#include "log/frame_logger_base.h"
#include "log/full_frame.h"
#include "state.h"

namespace deadeye {

class FrameLogger {
 public:
  FrameLogger() = default;

  FrameLogger(const int inum, const CaptureConfig& capture_config,
              const PipelineConfig& pipeline_config,
              const FrameLogConfig& log_config, FrameLoggerState& state) {
    switch (log_config.type) {
      case FrameLogType::capture:
        logger_ = std::make_unique<logger::FullFrame>(
            inum, capture_config, log_config, state, queue_, cancel_);
        break;

      case FrameLogType::four_up:
        logger_ = std::make_unique<logger::FourUp>(inum, capture_config,
                                                   pipeline_config, log_config,
                                                   state, queue_, cancel_);

      default:
        break;
    }
  };

  void Run() {
    future_ = std::async(std::launch::async, &logger::FrameLoggerBase::Run,
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
  std::unique_ptr<logger::FrameLoggerBase> logger_;
  std::future<void> future_;
  logger::FrameLoggerQueue queue_;
};

}  // namespace deadeye
