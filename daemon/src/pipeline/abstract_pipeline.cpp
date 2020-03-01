#include "abstract_pipeline.h"

#include <readerwriterqueue.h>
#include <spdlog/spdlog.h>
#include <wpi/Logger.h>
#include <future>

#include "config/deadeye_config.h"
#include "link/link.h"
#include "pipeline/pipeline_logger.h"
#include "pipeline/pipeline_ops.h"

using namespace deadeye;

namespace {
static const cv::Size kStreamSize{320, 240};
void InitializeLogging();
}  // namespace

AbstractPipeline::AbstractPipeline(int inum)
    : Pipeline{inum}, id_(DEADEYE_UNIT + std::to_string(inum)) {
  InitializeLogging();
}

namespace {
static int gcd(int a, int b) { return (b == 0) ? a : gcd(b, a % b); }
}  // namespace

/**
 * ConfigCapture handles changes to capture settings and only takes effect
 * after pipeline restart.
 */
void AbstractPipeline::ConfigCapture(CaptureConfig const &config) {
  safe::WriteAccess<LockableCaptureConfig> cc{capture_config_};
  *cc = config;
  pipeline_type_ = cc->PipelineType();
  fps_ = cc->frame_rate;
  int h = cc->OutputSize().height;
  int w = cc->OutputSize().width;
  preview_resize_ = w != kStreamSize.width;

  int r = gcd(w, h);
  int wr = w / r;
  int hr = h / r;
  spdlog::debug("{}: capture output: {}x{} ({}:{})", *this, w, h, wr, hr);

  if (wr == 4 && hr == 3) {  // 4:3
    preview_border_ = 0;
  } else if (wr == 16 && hr == 9) {
    preview_border_ = h / 6;
  } else {
    spdlog::error("{}: invalid aspect ratio: {}:{}", *this, wr, hr);
    preview_border_ = 0;
  }

  spdlog::debug("{}: {} {}", *this, *cc,
                preview_resize_ ? "[preview resized]" : "");
  capture_config_ready_ = true;
}

/**
 * ConfigPipeline handles changes to pipeline config.
 */
void AbstractPipeline::ConfigPipeline(PipelineConfig const &config) {
  safe::WriteAccess<LockablePipelineConfig> pc{pipeline_config_};
  *pc = config;
  spdlog::debug("{}: {}", *this, *pc);
  pipeline_config_ready_ = true;
}

/**
 * ConfigStream handles changes to video streaming.
 */
void AbstractPipeline::ConfigStream(StreamConfig const &config) {
  safe::WriteAccess<LockableStreamConfig> sc{stream_config_};
  *sc = config;
  spdlog::debug("{}: {}", *this, *sc);
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
  cs::MjpegServer mjpegServer{"cvhttpserver", 5805 + inum_};
  mjpegServer.SetSource(cvsource_);
  spdlog::info("{} streaming on port {}", *this, mjpegServer.GetPort());

  Link link{inum_};
  cv::Scalar hsv_low, hsv_high;
  FilterConfig filter;
  cv::TickMeter tm;
  PipelineLoggerQueue log_queue;
  int log_counter = 0;
  bool is_yuv;

  // Start frame logging thread
  CaptureConfig capture_config;
  PipelineConfig pipeline_config;
  {
    safe::ReadAccess<LockablePipelineConfig> pc{pipeline_config_};
    pipeline_config = *pc;
    safe::ReadAccess<LockableCaptureConfig> cc{capture_config_};
    capture_config = *cc;
    is_yuv = capture_config.type == CaptureConfig::Type::jetson;
  }
  auto lfuture = std::async(
      std::launch::async,
      PipelineLogger(id_, capture_config, pipeline_config, log_queue, cancel_));

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
      hsv_low = pc->HsvLow();
      hsv_high = pc->HsvHigh();
      filter = pc->filter;
      log_enabled_ = pc->log.fps > 0;
      log_interval_ = log_enabled_ ? fps_ / pc->log.fps : 0;
      log_counter = log_interval_;
      pipeline_config_ready_ = false;
      spdlog::debug("{}:{}", *this, *pc);
    }

    if (is_yuv) cv::cvtColor(frame_, frame_, cv::COLOR_YUV2BGR_I420);

    DE_IN_RANGE(frame_, hsv_low, hsv_high, hsv_threshold_output_);
    DE_FIND_CONTOURS(hsv_threshold_output_, find_contours_output_);

    FilterContours(filter, find_contours_output_, filter_contours_output_);
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
      log_queue.enqueue(PipelineLogEntry{frame_, filter_contours_output_,
                                         std::move(target_data_)});
      log_counter = log_interval_;
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
        preview = cv::Mat::zeros(kStreamSize, CV_8UC3);
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

  if (preview_border_ != 0) {
    cv::copyMakeBorder(preview, preview, preview_border_, preview_border_, 0, 0,
                       cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
  }

  if (preview_resize_)
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
