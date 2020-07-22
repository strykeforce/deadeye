#include "pipeline/runner.h"

#include <cscore.h>
#include <cscore_cv.h>
#include <wpi/Logger.h>

#include <future>
#include <opencv2/imgproc.hpp>

#include "config/deadeye_config.h"
#include "link/link.h"
#include "pipeline/gstreamer_capture.h"
#include "pipeline/logger.h"

using namespace deadeye;

namespace {
void InitializeLogging();
}  // namespace

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

  InitializeLogging();
  spdlog::info("{}: starting", *pipeline_);

  // load capture config at start of run
  int fps{0};
  pipeline_->Configure(capture_config_);
  fps = capture_config_.frame_rate;
  spdlog::debug("{}:{}", *pipeline_, capture_config_);

  // Set up streaming. CScore streaming will hang on connection if too many
  // connections are attempted, current workaround is for user to  disable and
  // reenable the stream to reset.
  cs::MjpegServer stream_server{"cvhttpserver", 5805 + pipeline_->GetInum()};
  cs::CvSource stream_source{"cvsource", cs::VideoMode::kMJPEG,
                             kStreamSize.width, kStreamSize.height, 30};
  stream_server.SetSource(stream_source);
  spdlog::info("{} streaming on port {}", *pipeline_, stream_server.GetPort());

  Link link{pipeline_->GetInum()};
  cv::Scalar hsv_low, hsv_high;
  FilterConfig filter;
  bool log_enabled = false;
  unsigned int log_counter{0};
  unsigned int sn{0};

  // start logger if used
  LoggerQueue log_queue;
  auto lfuture =
      std::async(std::launch::async,
                 Logger(DEADEYE_UNIT + std::to_string(pipeline_->GetInum()),
                        capture_config_, *pipeline_config_.readAccess(),
                        log_queue, cancel_));

  // start capture
  GstreamerCapture capture{capture_config_};

  cv::TickMeter tm;
  cv::Mat frame;
  bool stream_enabled{false};
  int log_interval{0};

  while (true) {  // Loop until pipeline cancelled

    if (cancel_.load()) {
      LogTickMeter(tm);
      return;
    }

    tm.start();

    // Check for updated configs
    if (pipeline_config_ready_.load()) {
      pipeline_config_ready_ = false;
      safe::ReadAccess<SafePipelineConfig> value{pipeline_config_};
      pipeline_->Configure(*value);
      log_enabled = value->log.fps > 0;
      log_interval = log_enabled ? fps / value->log.fps : 0;
      log_counter = log_interval;
      spdlog::debug("{}:{}", *pipeline_, *value);
    }

    if (stream_config_ready_.load()) {
      stream_config_ready_ = false;
      safe::ReadAccess<SafeStreamConfig> value{stream_config_};

      pipeline_->Configure(*value);
      stream_enabled = value->StreamEnabled();
      spdlog::debug("{}:{} [{}]", *pipeline_, *value,
                    stream_enabled ? "enabled" : "disabled");
    }

    // Get new frame
    if (!capture.Grab(frame)) {
      spdlog::critical("{} failed to grab frame", *pipeline_);
    }

    // Process frame through pipeline
    TargetDataPtr target_data = pipeline_->ProcessFrame(frame);
    target_data->serial = sn++;

    // Send target data to client
    link.Send(target_data.get());

    if (stream_enabled) {
      cv::Mat preview;
      pipeline_->ProcessStreamFrame(preview, target_data.get());
      stream_source.PutFrame(preview);
    }

    // Log frame if neccessary
    if (log_enabled && --log_counter == 0) {
      log_queue.enqueue(LogEntry{frame, pipeline_->GetFilteredContours(),
                                 std::move(target_data)});
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

namespace {
void InitializeLogging() {
  using namespace wpi;
  using namespace spdlog;
  static std::map<unsigned int, level::level_enum> levels{
      {WPI_LOG_DEBUG4, level::debug},     {WPI_LOG_DEBUG3, level::debug},
      {WPI_LOG_DEBUG2, level::debug},     {WPI_LOG_DEBUG1, level::debug},
      {WPI_LOG_DEBUG, level::debug},      {WPI_LOG_INFO, level::info},
      {WPI_LOG_WARNING, level::warn},     {WPI_LOG_ERROR, level::err},
      {WPI_LOG_CRITICAL, level::critical}};

  cs::SetLogger(
      [](unsigned int level, char const* file, unsigned int line,
         char const* msg) {
        spdlog::log(levels[level], "cscore: {} in {}, line {}", msg, file,
                    line);
      },
      WPI_LOG_WARNING);
}

}  // namespace
