#pragma once

#include "capture/gstreamer.h"
#include "config/capture_config.h"

namespace deadeye {
std::string BuildV4L2Pipeline(const CaptureConfig& config);

class UsbGStreamer : public Gstreamer {
 public:
  [[maybe_unused]] explicit UsbGStreamer(const CaptureConfig& config);
  ~UsbGStreamer() override = default;
};

}  // namespace deadeye
