#pragma once

#include "log/logger_impl.h"

namespace deadeye::logger {

class Capture : public LoggerImpl {
 public:
  Capture(std::string id, const CaptureConfig& capture_config, const LogConfig& log_config,
          LoggerQueue& queue, std::atomic<bool>& cancel);

  void Run() override;
};

}  // namespace deadeye
