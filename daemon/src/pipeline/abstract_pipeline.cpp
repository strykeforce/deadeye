#include "abstract_pipeline.h"

#include <spdlog/spdlog.h>
#include <wpi/Logger.h>

#include <opencv2/imgproc.hpp>

#include "config/deadeye_config.h"
#include "link/link.h"

using namespace deadeye;

namespace {
static const int kPreviewWidth = 320;
static const int kPreviewHeight = 240;
void InitializeLogging();
}  // namespace

AbstractPipeline::AbstractPipeline(int inum)
    : Pipeline{inum},
      id_(DEADEYE_UNIT + std::to_string(inum)),
      stream_config_ready_(false) {
  InitializeLogging();
}

AbstractPipeline::~AbstractPipeline() {}

/**
 * UpdateConfig handles changes to pipeline config.
 */
void AbstractPipeline::UpdateConfig(PipelineConfig const &config) {
  safe::WriteAccess<LockablePipelineConfig> pc{pipeline_config_};
  *pc = config;
  CaptureConfig gsc = pc->capture_config;
  pipeline_type_ = gsc.PipelineType();
  spdlog::info("{}:{}", *this, *pc);
  pipeline_config_ready_ = true;
}

/**
 * UpdateStream handles changes to video streaming.
 */
void AbstractPipeline::UpdateStream(StreamConfig const &config) {
  safe::WriteAccess<LockableStreamConfig> value{stream_config_};
  *value = config;
  spdlog::info("{}:{}", *this, *value);
  stream_config_ready_ = true;
}

void AbstractPipeline::CancelTask() { cancel_ = true; }

void AbstractPipeline::Run() {
  cancel_ = false;
  pipeline_config_ready_ = true;
  stream_config_ready_ = true;

  spdlog::info("Pipeline<{}>: starting", inum_);

  // Set up streaming. CScore streaming will hang on connection if too many
  // connections are attempted, current workaround is for user to  disable and
  // reenable the stream to reset.
  cvsource_ = cs::CvSource("cvsource", cs::VideoMode::kMJPEG, kPreviewWidth,
                           kPreviewHeight, 30);
  cs::MjpegServer mjpegServer{"cvhttpserver", 5800 + inum_};
  mjpegServer.SetSource(cvsource_);
  spdlog::info("{} streaming on port {}", *this, mjpegServer.GetPort());

  if (!StartCapture()) {
    spdlog::critical("{} failed to start video capture", *this);
    StopCapture();
    return;
  }

  Link link{inum_};
  cv::Scalar hsv_low, hsv_high;
  cv::TickMeter tm;

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

    // TODO: extract and cache the hi/lo cv:Scalars perhaps
    if (pipeline_config_ready_.load()) {
      safe::ReadAccess<LockablePipelineConfig> pc{pipeline_config_};
      hsv_low = cv::Scalar(pc->hue[0], pc->sat[0], pc->val[0]);
      hsv_high = cv::Scalar(pc->hue[1], pc->sat[1], pc->val[1]);
      pipeline_config_ready_ = false;
      spdlog::debug("{}:{}", *this, *pc);
    }

    cv::cvtColor(frame_, hsv_threshold_output_, cv::COLOR_BGR2HSV);
    cv::inRange(hsv_threshold_output_, hsv_low, hsv_high,
                hsv_threshold_output_);

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

    tm.stop();
  }
}

/////////////////////////////////////////////////////////////////////////////
// protected
/////////////////////////////////////////////////////////////////////////////

void AbstractPipeline::ProcessFrame(cv::Mat const &frame) {}

void AbstractPipeline::FilterContours(Contours const &src, Contours &dest) {}

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

  cv::resize(preview, preview, cv::Size(kPreviewWidth, kPreviewHeight), 0, 0,
             cv::INTER_AREA);
  cvsource_.PutFrame(preview);
}

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
