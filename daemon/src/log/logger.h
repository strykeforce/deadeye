#pragma once

#include <spdlog/spdlog.h>

#include <atomic>
#include <future>
#include <memory>
#include <opencv2/core/mat.hpp>

#include "log/four_up.h"
#include "log/logger_impl.h"

namespace deadeye {

class Logger {
 public:
  Logger() {}
  Logger(const std::string id, const CaptureConfig capture_config,
         const PipelineConfig pipeline_config) {
    logger_ = std::make_unique<FourUp>(id, capture_config, pipeline_config,
                                       queue_, cancel_);
  };
  void Run() {
    future_ = std::async(std::launch::async, &LoggerImpl::log, logger_.get());
    spdlog::debug("Logger::Run starting async logging task");
  }

  void Log(cv::Mat const frame, Contours filtered_contours,
           std::unique_ptr<TargetData> target) {
    queue_.enqueue(LogEntry{frame, filtered_contours, std::move(target)});
  }

  void Stop() {
    cancel_ = true;
    future_.wait();
    spdlog::debug("Logger::Stop stopped async logging task);
  }

 private:
  std::atomic<bool> cancel_{false};
  std::unique_ptr<LoggerImpl> logger_;
  std::future<void> future_;
  LoggerQueue queue_;
};

}  // namespace deadeye
