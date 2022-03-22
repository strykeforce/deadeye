#include "pipeline/runner.h"

#include <spdlog/spdlog.h>

#include <memory>
#include <opencv2/core/mat.hpp>

#include "capture/capture.h"
#include "capture/capture_factory.h"
#include "link/link.h"
#include "log/client_logger.h"
#include "log/frame_logger.h"
#include "pipeline/streamer.h"
#include "state.h"

using namespace deadeye;

void Runner::SetPipeline(std::unique_ptr<Pipeline> pipeline) {
  pipeline_ = std::move(pipeline);
}

Pipeline* Runner::GetPipeline() { return pipeline_.get(); }

void Runner::Configure(const CaptureConfig& config) {
  capture_config_ = config;
}

void Runner::Configure(const FrameLogConfig& config) { log_config_ = config; }

void Runner::Configure(const PipelineConfig& config) {
  safe::WriteAccess<SafePipelineConfig> value{pipeline_config_};
  *value = config;
  Configure(value->log);
  pipeline_config_ready_ = true;
}

void Runner::Configure(const StreamConfig& config) {
  safe::WriteAccess<SafeStreamConfig> value{stream_config_};
  *value = config;
  stream_config_ready_ = true;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NullDereference"
/**
 * Run loop
 */
void Runner::Run() {
  cancel_ = false;
  pipeline_config_ready_ = true;
  stream_config_ready_ = true;
  ClientLogger client_logger{pipeline_->GetInum()};
  PipelineState state = PipelineState::Load(pipeline_->GetInum());

  client_logger.Info(fmt::format("{}: starting", *pipeline_));

  // load capture config at start of run
  pipeline_->Configure(capture_config_);
  int fps = capture_config_.frame_rate;
  spdlog::debug("{}:{}", *pipeline_, capture_config_);

  // Set up streaming. CScore streaming will hang on connection if too many
  // connections are attempted, current workaround is for user to  disable and
  // re-enable the stream to reset.
  Streamer streamer(pipeline_.get(), capture_config_.Size());
  bool stream_enabled{false};

  Link link{pipeline_->GetInum()};
  bool frame_log_enabled = log_config_.fps > 0;

  std::unique_ptr<FrameLogger> frame_logger;
  if (frame_log_enabled) {
    frame_logger = std::make_unique<FrameLogger>(
        pipeline_->GetInum(), capture_config_, *pipeline_config_.readAccess(),
        log_config_, state.frame_logger);
    frame_logger->Run();
    spdlog::info("{}: logging enabled", *pipeline_);
  } else {
    spdlog::warn("{}: logging disabled", *pipeline_);
  }

  int log_interval = frame_log_enabled ? fps / log_config_.fps : 0;
  int log_counter = log_interval;
  int sn = 0;

  // start capture
  std::unique_ptr<Capture> capture = CreateCapture(capture_config_);
  cv::Mat frame;

  cv::TickMeter tm;

  while (true) {  // Loop until pipeline cancelled

    if (cancel_.load()) {
      double avg = tm.getTimeSec() / static_cast<double>(tm.getCounter());
      if (frame_log_enabled) frame_logger->Stop();
      state.Store();
      client_logger.Info(
          fmt::format("{}: stopped, avg. time = {:6.3f} ms, FPS = {:5.2f}",
                      *pipeline_, avg * 1000.0, 1.0 / avg));
      return;
    }

    tm.start();

    // Check for updated configs
    if (pipeline_config_ready_.load()) {
      pipeline_config_ready_ = false;
      safe::ReadAccess<SafePipelineConfig> value{pipeline_config_};
      auto config = *value;
      config.filter.frame_area = capture_config_.Size().area();
      pipeline_->Configure(config);
    }

    if (stream_config_ready_.load()) {
      stream_config_ready_ = false;
      safe::ReadAccess<SafeStreamConfig> value{stream_config_};

      streamer.Configure(*value);
      stream_enabled = value->StreamEnabled();
      spdlog::debug("{}:{}", *pipeline_, *value);
      spdlog::debug("{}:{} [{}]", *pipeline_, *value,
                    stream_enabled ? "enabled" : "disabled");
    }

    // Get new frame
    if (!capture->Grab(frame))
      client_logger.Error(fmt::format("{} failed to grab frame", *pipeline_));

    // Process frame through pipeline
    std::unique_ptr<TargetData> target_data = pipeline_->ProcessFrame(frame);
    target_data->serial = sn++;

    // Send target data to client
    link.Send(target_data.get());

    if (stream_enabled) streamer.Process(frame, target_data.get());

    // Log frame if necessary
    if (frame_log_enabled && --log_counter == 0) {
      frame_logger->Log(frame, pipeline_->GetFilteredContours(),
                        std::move(target_data));
      log_counter = log_interval;
    }

    tm.stop();
  }
}
#pragma clang diagnostic pop