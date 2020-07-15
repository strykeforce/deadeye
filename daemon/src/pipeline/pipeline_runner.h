#pragma once
#include <safe/safe.h>

#include <memory>
#include <opencv2/core/utility.hpp>

#include "pipeline/pipeline.h"

namespace deadeye {

using SafeCaptureConfig = safe::Safe<CaptureConfig>;
using SafePipelineConfig = safe::Safe<PipelineConfig>;
using SafeStreamConfig = safe::Safe<StreamConfig>;
using PipelinePtr = std::unique_ptr<Pipeline>;

class PipelineRunner {
 public:
  void SetPipeline(std::unique_ptr<Pipeline> pipeline);
  Pipeline *GetPipeline();

  void ConfigCapture(CaptureConfig const &config);
  void ConfigPipeline(PipelineConfig const &config);
  void ConfigStream(StreamConfig const &config);

  void Run();
  void Stop();

 private:
  void LogTickMeter(cv::TickMeter &tm);

  PipelinePtr pipeline_;

  std::atomic<bool> cancel_{false};

  SafeCaptureConfig capture_config_;
  std::atomic<bool> capture_config_ready_{false};
  SafeStreamConfig stream_config_;
  std::atomic<bool> stream_config_ready_{false};
  SafePipelineConfig pipeline_config_;
  std::atomic<bool> pipeline_config_ready_{false};
};
}  // namespace deadeye
