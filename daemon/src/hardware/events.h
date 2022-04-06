// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <tinyfsm.hpp>

#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"

namespace deadeye {
struct CameraOn : tinyfsm::Event {};
struct CameraOff : tinyfsm::Event {};

struct LightsOn : tinyfsm::Event {};
struct LightsOff : tinyfsm::Event {};
struct LightsBlink : tinyfsm::Event {};

struct ConfigCapture : tinyfsm::Event {
  CaptureConfig config;
};
struct ConfigPipeline : tinyfsm::Event {
  PipelineConfig config;
};
struct ConfigStream : tinyfsm::Event {
  StreamConfig config;
};

}  // namespace deadeye
