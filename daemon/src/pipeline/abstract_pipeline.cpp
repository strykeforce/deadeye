#include "abstract_pipeline.h"

#include <readerwriterqueue.h>
#include <spdlog/spdlog.h>
#include <wpi/Logger.h>
#include <future>
#include <opencv2/imgproc.hpp>

#include "config/deadeye_config.h"
#include "link/link.h"
#include "pipeline/pipeline_logger.h"

using namespace deadeye;

namespace {
static const cv::Size kStreamSize{320, 240};
void InitializeLogging();
}  // namespace

AbstractPipeline::AbstractPipeline(int inum)
    : Pipeline{inum}, id_(DEADEYE_UNIT + std::to_string(inum)) {
  InitializeLogging();
}

/**
 * ConfigCapture handles changes to capture settings and only takes effect
 * after pipeline restart.
 */
void AbstractPipeline::ConfigCapture(CaptureConfig const &config) {
  safe::WriteAccess<LockableCaptureConfig> cc{capture_config_};
  *cc = config;
  pipeline_type_ = config.PipelineType();
  fps_ = config.frame_rate;
  spdlog::debug("{}:{}", *this, *cc);
  capture_config_ready_ = true;
}

/**
 * ConfigPipeline handles changes to pipeline config.
 */
void AbstractPipeline::ConfigPipeline(PipelineConfig const &config) {
  safe::WriteAccess<LockablePipelineConfig> pc{pipeline_config_};
  *pc = config;
  spdlog::debug("{}:{}", *this, *pc);
  pipeline_config_ready_ = true;
}

/**
 * ConfigStream handles changes to video streaming.
 */
void AbstractPipeline::ConfigStream(StreamConfig const &config) {
  safe::WriteAccess<LockableStreamConfig> sc{stream_config_};
  *sc = config;
  spdlog::debug("{}:{}", *this, *sc);
  stream_config_ready_ = true;
}

void AbstractPipeline::CancelTask() { cancel_ = true; }

void AbstractPipeline::Run() {
  cancel_ = false;
  capture_config_ready_ = true;
  pipeline_config_ready_ = true;
  stream_config_ready_ = true;

  spdlog::info("{}: starting", *this);

  // Set up streaming. CScore streaming will hang on connection if too many
  // connections are attempted, current workaround is for user to  disable and
  // reenable the stream to reset.
  cvsource_ = cs::CvSource("cvsource", cs::VideoMode::kMJPEG, kStreamSize.width,
                           kStreamSize.height, 30);
  cs::MjpegServer mjpegServer{"cvhttpserver", 5800 + inum_};
  mjpegServer.SetSource(cvsource_);
  spdlog::info("{} streaming on port {}", *this, mjpegServer.GetPort());

  Link link{inum_};
  cv::Scalar hsv_low, hsv_high;
  cv::TickMeter tm;
  PipelineLoggerQueue log_queue;
  int log_counter = fps_;

  // Start frame logging thread
  LogConfig lc;
  {
    safe::ReadAccess<LockablePipelineConfig> pc{pipeline_config_};
    lc = pc->log;
  }
  auto lfuture = std::async(std::launch::async,
                            PipelineLogger(id_, lc, log_queue, cancel_));

  if (!StartCapture()) {
    spdlog::critical("{} failed to start video capture", *this);
    StopCapture();
    return;
  }

  for (int i = 0;; i++) {  // Loop until task cancelled.
    tm.start();

    // Check for cancellation of this pipeline.
    if (cancel_.load()) {
      StopCapture();
      LogTickMeter(tm);
      return;
    }

    // Get new frame and process it.
    if (!GrabFrame(frame_)) {
      spdlog::critical("{} failed to grab frame", *this);
    }

    if (pipeline_config_ready_.load()) {
      safe::ReadAccess<LockablePipelineConfig> pc{pipeline_config_};
      hsv_low = cv::Scalar(pc->hue[0], pc->sat[0], pc->val[0]);
      hsv_high = cv::Scalar(pc->hue[1], pc->sat[1], pc->val[1]);
      log_enabled_ = pc->log.enabled;
      pipeline_config_ready_ = false;
      spdlog::debug("{}:{}", *this, *pc);
    }

    cv::cvtColor(frame_, hsv_threshold_output_, cv::COLOR_BGR2HSV);
    cv::inRange(hsv_threshold_output_, hsv_low, hsv_high,
                hsv_threshold_output_);

    find_contours_output_.clear();
    cv::findContours(hsv_threshold_output_, find_contours_output_,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // spdlog::debug("Contours found: {}", find_contours_output_.size());

    FilterContours(find_contours_output_, filter_contours_output_);
    target_data_ = ProcessTarget(filter_contours_output_);
    target_data_->serial = i;
    link.Send(target_data_.get());

    if (stream_config_ready_.load()) {
      safe::ReadAccess<LockableStreamConfig> sc{stream_config_};
      view_ = sc->view;
      contour_ = sc->contour;
      stream_config_ready_ = false;
      spdlog::debug("{}:{}", *this, *sc);
    }

    if (StreamEnabled()) StreamFrame();

    if (log_enabled_ && --log_counter == 0) {
      log_queue.enqueue(
          PipelineLogEntry{frame_, hsv_threshold_output_, find_contours_output_,
                           filter_contours_output_, std::move(target_data_)});
      log_counter = fps_;
    }

    tm.stop();
  }
}

/////////////////////////////////////////////////////////////////////////////
// private
/////////////////////////////////////////////////////////////////////////////

void AbstractPipeline::StreamFrame() {
  using View = StreamConfig::View;
  using Contour = StreamConfig::Contour;

  cv::Mat preview;
  switch (view_) {
    case View::none:
      if (contour_ != Contour::none) {
        preview = cv::Mat::zeros(frame_.size(), CV_8UC3);
      }
      break;
    case View::original:
      if (contour_ != Contour::none) {
        cv::cvtColor(frame_, preview, cv::COLOR_BGR2GRAY);
        cv::cvtColor(preview, preview, cv::COLOR_GRAY2BGR);
        break;
      }
      preview = frame_;
      break;
    case View::mask:
      cv::cvtColor(hsv_threshold_output_, preview, cv::COLOR_GRAY2BGR);
      break;
  }

  switch (contour_) {
    case Contour::none:
      break;
    case Contour::filter:
      cv::drawContours(preview, filter_contours_output_, -1,
                       cv::Scalar(255, 0, 240), 2);
      target_data_->DrawMarkers(preview);
      break;

    case Contour::all:
      cv::drawContours(preview, find_contours_output_, -1,
                       cv::Scalar(255, 0, 240), 2);
      break;
  }

  if (preview.cols != kStreamSize.width)
    cv::resize(preview, preview, kStreamSize, 0, 0, cv::INTER_AREA);
  cvsource_.PutFrame(preview);
}

void AbstractPipeline::LogFrame() {}

void AbstractPipeline::LogTickMeter(cv::TickMeter tm) {
  spdlog::info("{}: stopping", *this);
  double avg = tm.getTimeSec() / tm.getCounter();
  double fps = 1.0 / avg;
  spdlog::info("{}: avg. time = {:6.3f} ms, FPS = {:5.2f}", *this, avg * 1000.0,
               fps);
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
      [](unsigned int level, char const *file, unsigned int line,
         char const *msg) {
        spdlog::log(levels[level], "cscore: {} in {}, line {}", msg, file,
                    line);
      },
      WPI_LOG_WARNING);
}

}  // namespace
