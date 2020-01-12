#include "driver_pipeline.h"

#include <cscore.h>
#include <cscore_cv.h>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <wpi/Logger.h>
#include <map>
#include <opencv2/core/mat.hpp>

#include "config/deadeye_config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "link/link.h"

using namespace deadeye;

/**
 * constructor configures cscore logging.
 */
DriverPipeline::DriverPipeline(int inum)
    : Pipeline{inum}, id_(DEADEYE_UNIT + std::to_string(inum)) {
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

void DriverPipeline::CancelTask() { cancel_ = true; }

void DriverPipeline::ConfigPipeline(PipelineConfig const &config) {
  pipeline_config_ = config;
  spdlog::debug("DriverPipeline<{}>: {}", inum_, pipeline_config_);
}

void DriverPipeline::ConfigStream(StreamConfig const &config) {
  stream_enabled_ = config.view == StreamConfig::View::original;
  spdlog::debug("DriverPipeline<{}> stream enabled: {}", inum_,
                stream_enabled_.load());
}

cv::VideoCapture DriverPipeline::GetVideoCapture() {
#ifdef __APPLE__
  std::string pipeline = "autovideosrc ! videoconvert ! appsink";
#else
  // std::string pipeline{pipeline_config_.capture_config.Pipeline()};
  // spdlog::debug("{}: {}", *this, pipeline);
  std::string pipeline{""};
#endif
  cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
  return cap;
}

void DriverPipeline::Run() {
  cancel_ = false;
  spdlog::info("DriverPipeline<{}>: starting", inum_);

  cv::Mat frame;
  cv::VideoCapture cap = GetVideoCapture();

  if (!cap.isOpened()) {
    spdlog::critical(
        "DriverPipeline<{}>: unable to open camera({}) in {}, line {}", inum_,
        inum_, __FILE__, __LINE__);
    throw PipelineException("unable to open camera");
  }

  cs::CvSource cvsource;
  // Set up streaming. CScore streaming will hang on connection if too many
  // connections are attempted, current workaround is for user to  disable and
  // reenable the stream to reset.
  cvsource = cs::CvSource{"cvsource", cs::VideoMode::kMJPEG, 320, 240, 30};
  cs::MjpegServer mjpegServer{"cvhttpserver", 5800 + inum_};
  mjpegServer.SetSource(cvsource);
  spdlog::info("{} streaming on port {}", *this, inum_, mjpegServer.GetPort());

  Link link{inum_};

  // Loop until task cancelled.
  cv::TickMeter tm;
  for (int i = 0;; i++) {
    tm.start();

    if (cancel_.load()) {
      spdlog::info("{}: stopping", *this);
      double avg = tm.getTimeSec() / tm.getCounter();
      double fps = 1.0 / avg;
      spdlog::info("{}: avg. time = {:6.3f} ms, FPS = {:5.2f}", *this,
                   avg * 1000.0, fps);
      return;
    }

    cap >> frame;

    if (stream_enabled_.load()) {
      cv::Mat preview = frame;
      if (frame.cols > 320) {
        cv::resize(frame, preview, cv::Size(320, 180));
      }
      cv::copyMakeBorder(preview, preview, 30, 30, 0, 0, cv::BORDER_CONSTANT,
                         cv::Scalar(0, 0, 0));

      cvsource.PutFrame(preview);
    }

    link.Send(std::make_unique<TargetData>(id_, i, false).get());
    tm.stop();
  }
}

std::string DriverPipeline::ToString() const {
  return fmt::format("DriverPipeline<{}>", inum_);
}
