#include "driver_pipeline.hpp"

#include <cscore.h>
#include <spdlog/spdlog.h>
#include <wpi/Logger.h>
#include <map>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>

#define PREVIEW_WIDTH 320
#define PREVIEW_HEIGHT 240

using namespace deadeye;

/**
 * constructor configures cscore logging.
 */
DriverPipeline::DriverPipeline(int inum) : Pipeline{inum} {
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

void DriverPipeline::UpdateConfig(PipelineConfig *config) {
  spdlog::warn("PipelineConfig updates ignored in DriverPipeline");
}

void DriverPipeline::UpdateStream(StreamConfig *config) {
  spdlog::warn("StreamConfig updates ignored in DriverPipeline");
}

namespace {
#ifndef __APPLE__
std::string gstreamer_pipeline(int capture_width, int capture_height,
                               int display_width, int display_height,
                               int framerate, int flip_method) {
  return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" +
         std::to_string(capture_width) + ", height=(int)" +
         std::to_string(capture_height) +
         ", format=(string)NV12, framerate=(fraction)" +
         std::to_string(framerate) +
         "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) +
         " ! video/x-raw, width=(int)" + std::to_string(display_width) +
         ", height=(int)" + std::to_string(display_height) +
         ", format=(string)BGRx ! videoconvert ! video/x-raw, "
         "format=(string)BGR ! appsink";
}
#endif

cv::VideoCapture GetVideoCapture() {
#ifdef __APPLE__
  cv::VideoCapture cap{0, cv::CAP_AVFOUNDATION};
#else
  int capture_width = 1280;
  int capture_height = 720;
  int display_width = 1280;
  int display_height = 720;
  int framerate = 60;
  int flip_method = 0;

  std::string pipeline =
      gstreamer_pipeline(capture_width, capture_height, display_width,
                         display_height, framerate, flip_method);
  spdlog::info("Using gstreamer pipeline: {}", pipeline);

  cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
#endif
  return cap;
}

}  // namespace

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

  // Set up streaming. CScore streaming will hang on connection if too many
  // connections are attempted, current workaround is for user to  disable and
  // reenable the stream to reset.
  cs::CvSource cvsource{"cvsource", cs::VideoMode::kMJPEG, PREVIEW_WIDTH,
                        PREVIEW_HEIGHT, 30};
  cs::MjpegServer mjpegServer{"cvhttpserver", 5800 + inum_};
  mjpegServer.SetSource(cvsource);
  spdlog::info("DriverPipeline<{}> streaming on port {}", inum_,
               mjpegServer.GetPort());

  // Loop until task cancelled.
  for (cv::TickMeter tm;;) {
    tm.start();

    if (cancel_.load()) {
      spdlog::info("Pipeline<{}>: stopping", inum_);
      double avg = tm.getTimeSec() / tm.getCounter();
      double fps = 1.0 / avg;
      spdlog::info("Pipeline<{}>: avg. time = {}, FPS = {}", inum_, avg, fps);
      return;
    }

    cap >> frame;

    cv::Mat preview;
    cv::copyMakeBorder(frame, preview, 120, 120, 0, 0, cv::BORDER_CONSTANT,
                       cv::Scalar(0, 0, 0));

    cv::resize(preview, preview, cv::Size(PREVIEW_WIDTH, PREVIEW_HEIGHT), 0, 0,
               cv::INTER_AREA);
    cvsource.PutFrame(preview);

    tm.stop();
  }
}
