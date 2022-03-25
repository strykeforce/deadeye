// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include "capture/gstreamer.h"
#include "config/capture_config.h"

namespace deadeye {
class VideoTest : public Gstreamer {
 public:
  explicit VideoTest(const CaptureConfig& config);
  ~VideoTest() override = default;
};

}  // namespace deadeye
