#pragma once

#include "capture/gstreamer.h"
#include "config/capture_config.h"

namespace deadeye {
class VideoTest : public Gstreamer {
 public:
  VideoTest(const CaptureConfig& config);
  virtual ~VideoTest() {}
};

}  // namespace deadeye
