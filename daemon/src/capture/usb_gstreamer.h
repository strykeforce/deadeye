#pragma once

#include "capture/gstreamer.h"
#include "config/capture_config.h"

namespace deadeye {
std::string BuildV4L2Pipeline(const CaptureConfig& config);

class UsbGStreamer : public Gstreamer {
 public:
  UsbGStreamer(const CaptureConfig& config);
  virtual ~UsbGStreamer() {}
};

}  // namespace deadeye
