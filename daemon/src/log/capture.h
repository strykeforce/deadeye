#pragma once

#include "log/logger_impl.h"

namespace deadeye {
namespace logger {

class Capture : public LoggerImpl {
 public:
  Capture(std::string id, CaptureConfig capture_config, LogConfig log_config,
          LoggerQueue& queue, std::atomic<bool>& cancel);

  void Run() override;
};

}  // namespace logger
}  // namespace deadeye
