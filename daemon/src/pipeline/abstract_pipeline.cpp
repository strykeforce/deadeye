#include "abstract_pipeline.hpp"

#include <cscore.h>
#include <spdlog/spdlog.h>
#include <wpi/Logger.h>

#include <opencv2/imgproc.hpp>

#include "config/deadeye_config.hpp"
#include "link/center_target_data.hpp"
#include "link/link.hpp"

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
  safe::WriteAccess<LockablePipelineConfig> value{pipeline_config_};
  *value = config;
  spdlog::info("{}:{}", *this, *value);
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
  cs::CvSource cvsource{"cvsource", cs::VideoMode::kMJPEG, kPreviewWidth,
                        kPreviewHeight, 30};
  cs::MjpegServer mjpegServer{"cvhttpserver", 5800 + inum_};
  mjpegServer.SetSource(cvsource);
  spdlog::info("{} streaming on port {}", *this, mjpegServer.GetPort());

  cv::Mat frame;
  Link link{inum_};
  StreamConfig stream;
  PipelineConfig config;

  if (!StartCapture()) {
    spdlog::critical("{} failed to start video capture", *this);
    StopCapture();
    return;
  }

  // Loop until task cancelled.
  cv::TickMeter tm;
  for (int i = 0;; i++) {
    tm.start();

    // Check for cancellation of this task.
    if (cancel_.load()) {
      StopCapture();
      LogTickMeter(tm);
      return;
    }

    // Get new frame and process it.
    if (!GrabFrame(frame)) {
      spdlog::critical("{} failed to grab frame", *this);
    }

    // TODO: extract and cache the hi/lo cv:Scalars perhaps
    if (pipeline_config_ready_.load()) {
      safe::ReadAccess<LockablePipelineConfig> value{pipeline_config_};
      config = *value;
      pipeline_config_ready_ = false;
      spdlog::debug("{}:{}", *this, *value);
    }

    cv::Mat hsv_threshold_output;
    cv::cvtColor(frame, hsv_threshold_output, cv::COLOR_BGR2HSV);
    cv::inRange(hsv_threshold_output,
                cv::Scalar(config.hue[0], config.sat[0], config.val[0]),
                cv::Scalar(config.hue[1], config.sat[1], config.val[1]),
                hsv_threshold_output);

    Contours find_contours_output;
    cv::findContours(hsv_threshold_output, find_contours_output,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // spdlog::debug("Contours found: {}", find_contours_output.size());

    Contours filter_contours_output;
    FilterContours(find_contours_output, filter_contours_output);
    TargetDataPtr td = ProcessTarget(filter_contours_output);
    td->serial = i;
    link.Send(td.get());

    if (stream_config_ready_.load()) {
      safe::ReadAccess<LockableStreamConfig> value{stream_config_};
      stream = *value;
      stream_config_ready_ = false;
      spdlog::debug("{}:{}", *this, *value);
    }

    if (stream.view != StreamConfig::View::NONE ||
        stream.contour != StreamConfig::Contour::NONE) {
      cv::Mat preview;
      switch (stream.view) {
        case StreamConfig::View::NONE:
          if (stream.contour != StreamConfig::Contour::NONE) {
            preview = cv::Mat::zeros(frame.size(), CV_8UC3);
          }
          break;
        case StreamConfig::View::ORIGINAL:
          if (stream.contour != StreamConfig::Contour::NONE) {
            cv::cvtColor(frame, preview, cv::COLOR_BGR2GRAY);
            cv::cvtColor(preview, preview, cv::COLOR_GRAY2BGR);
            break;
          }
          preview = frame;
          break;
        case StreamConfig::View::MASK:
          cv::cvtColor(hsv_threshold_output, preview, cv::COLOR_GRAY2BGR);
          break;
      }

      switch (stream.contour) {
        case StreamConfig::Contour::NONE:
          break;
        case StreamConfig::Contour::FILTER:
          cv::drawContours(preview, filter_contours_output, -1,
                           cv::Scalar(255, 0, 240), 2);
          td->DrawMarkers(preview);
          break;

        case StreamConfig::Contour::ALL:
          cv::drawContours(preview, find_contours_output, -1,
                           cv::Scalar(255, 0, 240), 2);
          break;
      }

      cv::resize(preview, preview, cv::Size(kPreviewWidth, kPreviewHeight), 0,
                 0, cv::INTER_AREA);
      cvsource.PutFrame(preview);
    }

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
