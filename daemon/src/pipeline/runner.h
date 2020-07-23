#pragma once
#include <safe/safe.h>

#include <memory>
#include <opencv2/core/utility.hpp>

#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "pipeline/pipeline.h"

namespace deadeye {

using SafePipelineConfig = safe::Safe<PipelineConfig>;
using SafeStreamConfig = safe::Safe<StreamConfig>;
using PipelinePtr = std::unique_ptr<Pipeline>;

class Runner {
 public:
  void SetPipeline(std::unique_ptr<Pipeline> pipeline);
  Pipeline* GetPipeline();

  void Configure(CaptureConfig config);
  void Configure(PipelineConfig const& config);
  void Configure(StreamConfig const& config);

  void Run();
  void Stop();

 private:
  void LogTickMeter(cv::TickMeter& tm);

  PipelinePtr pipeline_;

  std::atomic<bool> cancel_{false};

  CaptureConfig capture_config_;
  SafeStreamConfig stream_config_;
  std::atomic<bool> stream_config_ready_{false};
  SafePipelineConfig pipeline_config_;
  std::atomic<bool> pipeline_config_ready_{false};
};
}  // namespace deadeye
