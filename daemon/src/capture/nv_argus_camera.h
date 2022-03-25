// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <string>

#include "capture/gstreamer.h"
#include "config/capture_config.h"

namespace deadeye {
std::string NvArgusCameraPipeline(const CaptureConfig& config);

class NvArgusCamera : public Gstreamer {
 public:
  explicit NvArgusCamera(const CaptureConfig& config);
  ~NvArgusCamera() override = default;
};

}  // namespace deadeye
