// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <spdlog/spdlog.h>

#include <memory>

#include "capture/capture.h"
#include "capture/capture_type.h"
#include "capture/image_file.h"
#include "capture/nv_argus_camera.h"
#include "capture/usb_cscore.h"
#include "capture/usb_gstreamer.h"
#include "capture/video_test.h"

namespace deadeye {
static std::unique_ptr<Capture> CreateCapture(const CaptureConfig& config) {
  switch (config.type) {
    case CaptureType::test:
      return std::make_unique<VideoTest>(config);
    case CaptureType::jetson:
      return std::make_unique<NvArgusCamera>(config);
    case CaptureType::file:
      return std::make_unique<ImageFile>(config);
    case CaptureType::usb:
      // return std::make_unique<UsbGStreamer>(config);
      return std::make_unique<UsbCsCore>(config);

    default:
      spdlog::critical("CreateCapture: unimplemented type: {}",
                       config.PipelineType());
      return nullptr;
  }
}

}  // namespace deadeye
