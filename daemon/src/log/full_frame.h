#pragma once

#include "log/frame_logger_base.h"

namespace deadeye::logger {

class FullFrame : public FrameLoggerBase {
 public:
  FullFrame(int inum, const CaptureConfig& capture_config,
            const FrameLogConfig& log_config, FrameLoggerQueue& queue,
            std::atomic<bool>& cancel);

  void RunLoop() override;
};

}  // namespace deadeye::logger
