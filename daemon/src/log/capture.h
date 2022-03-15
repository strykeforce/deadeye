#pragma once

#include "log/frame_logger_impl.h"

namespace deadeye::logger {

class Capture : public FrameLoggerImpl {
 public:
  Capture(std::string id, const CaptureConfig& capture_config,
          const FrameLogConfig& log_config, LoggerQueue& queue,
          std::atomic<bool>& cancel);

  void Run() override;
};

}  // namespace deadeye::logger
