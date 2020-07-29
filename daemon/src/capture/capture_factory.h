#pragma once

#include <spdlog/spdlog.h>

#include "capture/capture.h"
#include "capture/capture_type.h"
#include "capture/nv_argus_camera.h"
#include "capture/video_test.h"

namespace deadeye {
static std::unique_ptr<Capture> CreateCapture(const CaptureConfig& config) {
  switch (config.type) {
    case CaptureType::test:
      return std::make_unique<VideoTest>(config);
    case CaptureType::jetson:
      return std::make_unique<NvArgusCamera>(config);

    default:
      spdlog::critical("CreateCapture: unimplemented type: {}",
                       config.PipelineType());
      return nullptr;
  }
}

}  // namespace deadeye