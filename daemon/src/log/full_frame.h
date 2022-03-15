#pragma once

#include "log/frame_logger_base.h"

namespace deadeye::logger {

class FullFrame : public FrameLoggerBase {
 public:
  FullFrame(const int inum, const CaptureConfig& capture_config,
          const FrameLogConfig& log_config, FrameLoggerQueue& queue,
          std::atomic<bool>& cancel);

  void Run() override;
};

}  // namespace deadeye::logger
