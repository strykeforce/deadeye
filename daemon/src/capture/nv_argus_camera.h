#pragma once

#include "capture/gstreamer.h"
#include "config/capture_config.h"

namespace deadeye {
std::string NvArgusCameraPipeline(const CaptureConfig& config);

class NvArgusCamera : public Gstreamer {
 public:
  NvArgusCamera(const CaptureConfig& config);
  virtual ~NvArgusCamera() {}
};

}  // namespace deadeye
