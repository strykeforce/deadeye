#include "pipeline/runner.h"

#include <spdlog/spdlog.h>

#include <memory>
#include <opencv2/core/mat.hpp>

#include "capture/capture.h"
#include "capture/capture_factory.h"
#include "config/deadeye_config.h"
#include "link/link.h"
#include "log/logger.h"
#include "pipeline/streamer.h"

using namespace deadeye;

void Runner::SetPipeline(std::unique_ptr<Pipeline> pipeline) {
  pipeline_ = std::move(pipeline);
}

Pipeline* Runner::GetPipeline() { return pipeline_.get(); }

void Runner::Configure(CaptureConfig config) { capture_config_ = config; }

void Runner::Configure(PipelineConfig const& config) {
  safe::WriteAccess<SafePipelineConfig> value{pipeline_config_};
  *value = config;
  pipeline_config_ready_ = true;
}

void Runner::Configure(StreamConfig const& config) {
  safe::WriteAccess<SafeStreamConfig> value{stream_config_};
  *value = config;
  stream_config_ready_ = true;
}

/**
 * Run loop
 */
void Runner::Run() {
  cancel_ = false;
  pipeline_config_ready_ = true;
  stream_config_ready_ = true;

  spdlog::info("{}: starting", *pipeline_);

  // load capture config at start of run
  pipeline_->Configure(capture_config_);
  int fps = capture_config_.frame_rate;
  spdlog::debug("{}:{}", *pipeline_, capture_config_);

  // Set up streaming. CScore streaming will hang on connection if too many
  // connections are attempted, current workaround is for user to  disable and
  // reenable the stream to reset.
  Streamer streamer(pipeline_.get(), capture_config_.Size());
  bool stream_enabled{false};

  Link link{pipeline_->GetInum()};
  bool log_enabled = pipeline_config_.readAccess()->log.fps > 0;

  std::unique_ptr<Logger> logger = nullptr;
  if (log_enabled) {
    logger.reset(new Logger(CameraId(pipeline_->GetInum()), capture_config_,
                            *pipeline_config_.readAccess()));
    logger->Run();
    spdlog::info("{}: logging enabled", *pipeline_);
  } else {
    spdlog::warn("{}: logging disabled", *pipeline_);
  }

  unsigned int log_counter{0};
  unsigned int sn{0};
  int log_interval{0};

  // start capture
  std::unique_ptr<Capture> capture = CreateCapture(capture_config_);
  cv::Mat frame;

  cv::TickMeter tm;

  while (true) {  // Loop until pipeline cancelled

    if (cancel_.load()) {
      LogTickMeter(tm);
      if (log_enabled) logger->Stop();
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
      log_interval = log_enabled ? fps / config.log.fps : 0;
    }

    if (stream_config_ready_.load()) {
      stream_config_ready_ = false;
      safe::ReadAccess<SafeStreamConfig> value{stream_config_};

      streamer.Configure(*value);
      stream_enabled = value->StreamEnabled();
      log_counter = log_interval;
      spdlog::debug("{}:{}", *pipeline_, *value);
      spdlog::debug("{}:{} [{}]", *pipeline_, *value,
                    stream_enabled ? "enabled" : "disabled");
    }

    // Get new frame
    if (!capture->Grab(frame))
      spdlog::critical("{} failed to grab frame", *pipeline_);

    // Process frame through pipeline
    std::unique_ptr<TargetData> target_data = pipeline_->ProcessFrame(frame);
    target_data->serial = sn++;

    // Send target data to client
    link.Send(target_data.get());

    if (stream_enabled) streamer.Process(frame, target_data.get());

    // Log frame if neccessary
    if (log_enabled && --log_counter == 0) {
      logger->Log(frame, pipeline_->GetFilteredContours(),
                  std::move(target_data));
      log_counter = log_interval;
    }

    tm.stop();
  }
}

void Runner::Stop() { cancel_ = true; }

/////////////////////////////////////////////////////////////////////////////
// private
/////////////////////////////////////////////////////////////////////////////

void Runner::LogTickMeter(cv::TickMeter& tm) {
  spdlog::info("{}: stopping", *pipeline_);
  double avg = tm.getTimeSec() / tm.getCounter();
  double fps = 1.0 / avg;
  spdlog::info("{}: avg. time = {:6.3f} ms, FPS = {:5.2f}", *pipeline_,
               avg * 1000.0, fps);
}
