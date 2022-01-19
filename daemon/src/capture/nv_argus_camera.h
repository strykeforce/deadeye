#pragma once

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
