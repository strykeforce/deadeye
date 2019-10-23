#include "abstract_pipeline.hpp"

#include <cscore.h>
#include <spdlog/spdlog.h>
#include <wpi/Logger.h>

#include <opencv2/imgproc.hpp>

#include "config/deadeye_config.hpp"
#include "config/pipeline_config.hpp"
#include "config/stream_config.hpp"
#include "link/center_target_data.hpp"
#include "link/link.hpp"

using namespace deadeye;

namespace {
static const int kPreviewWidth = 320;
static const int kPreviewHeight = 240;
void InitializeLogging();
}  // namespace

AbstractPipeline::AbstractPipeline(int inum)
    : Pipeline{inum}, id_(DEADEYE_UNIT + std::to_string(inum)) {
  InitializeLogging();
}

AbstractPipeline::~AbstractPipeline() {}

void AbstractPipeline::UpdateConfig(PipelineConfig *config) {
  // The config is read for every frame but updated very infrequently.
  // ProcessFrame accesses the current config throught the atomic pointer
  // pipeline_config_. To prevent deleting the config ProcessFrame is using
  // during this update, the previous config is retained and pointed to by
  // prev_pipeline_config_. It is then deleted and replaced the next time the
  // config is updated. This assumes that ProcessFrame will finish using the
  // config before this update is called again, a safe assumption since
  // updates are user initiated via the web admin UI.
  delete prev_pipeline_config_;
  prev_pipeline_config_ = pipeline_config_.load();
  pipeline_config_.store(config);
  spdlog::debug("{}: {}", *this, *(pipeline_config_.load()));
}

/**
 * UpdateStream handles changes to video streaming.
 */
void AbstractPipeline::UpdateStream(StreamConfig *config) {
  // Same algorithm as UpdateConfig above.
  delete prev_stream_config_;
  prev_stream_config_ = stream_config_.load();
  stream_config_.store(config);
  spdlog::debug("{}: {}", *this, *(stream_config_.load()));
}

void AbstractPipeline::CancelTask() { cancel_ = true; }

void AbstractPipeline::Run() {
  cancel_ = false;
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
    PipelineConfig *config = pipeline_config_.load();

    cv::Mat hsv_threshold_output;
    cv::cvtColor(frame, hsv_threshold_output, cv::COLOR_BGR2HSV);
    cv::inRange(hsv_threshold_output,
                cv::Scalar(config->hue[0], config->sat[0], config->val[0]),
                cv::Scalar(config->hue[1], config->sat[1], config->val[1]),
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

    StreamConfig *stream = stream_config_.load();
    if (stream->view != StreamConfig::View::NONE ||
        stream->contour != StreamConfig::Contour::NONE) {
      cv::Mat preview;
      switch (stream->view) {
        case StreamConfig::View::NONE:
          if (stream->contour != StreamConfig::Contour::NONE) {
            preview = cv::Mat::zeros(frame.size(), CV_8UC3);
          }
          break;
        case StreamConfig::View::ORIGINAL:
          if (stream->contour != StreamConfig::Contour::NONE) {
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

      switch (stream->contour) {
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
