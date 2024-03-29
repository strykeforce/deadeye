// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <safe/safe.h>

#include <memory>
#include <opencv2/core/utility.hpp>

#include "config/capture_config.h"
#include "config/frame_log_config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "pipeline/pipeline.h"

namespace deadeye {

using SafePipelineConfig = safe::Safe<PipelineConfig>;
using SafeStreamConfig = safe::Safe<StreamConfig>;
using PipelinePtr = std::unique_ptr<Pipeline>;

class Runner {
 public:
  Runner() = default;
  ~Runner() = default;
  void SetPipeline(std::unique_ptr<Pipeline> pipeline);
  Pipeline* GetPipeline();

  void Configure(const CaptureConfig& config);
  void Configure(const FrameLogConfig& config);
  void Configure(const PipelineConfig& config);
  void Configure(const StreamConfig& config);
  void SetLoggingEnabled(bool enabled);

  void Run();
  void Stop() { cancel_ = true; }

 private:
  PipelinePtr pipeline_;

  std::atomic<bool> cancel_{false};

  CaptureConfig capture_config_;
  FrameLogConfig log_config_;
  SafeStreamConfig stream_config_;
  std::atomic<bool> stream_config_ready_{false};
  SafePipelineConfig pipeline_config_;
  std::atomic<bool> pipeline_config_ready_{false};
  std::atomic<bool> logging_enabled_{false};
};
}  // namespace deadeye
