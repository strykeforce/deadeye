// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <string>

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
