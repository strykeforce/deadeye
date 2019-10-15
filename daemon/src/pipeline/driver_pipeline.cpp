#include "driver_pipeline.hpp"

#include <cscore.h>
#include <spdlog/spdlog.h>
#include <wpi/Logger.h>
#include <map>
#include <opencv2/core/mat.hpp>
#include "link/link.hpp"

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

DriverPipeline::~DriverPipeline() {
  if (pipeline_config_ != nullptr) delete pipeline_config_;
}

void DriverPipeline::UpdateConfig(PipelineConfig *config) {
  if (pipeline_config_ == nullptr) {
    pipeline_config_ = config;
    spdlog::info("DriverPipeline<{}> new config: {}", inum_, *pipeline_config_);
  } else {
    spdlog::warn("PipelineConfig updates ignored in DriverPipeline");
  }
}

void DriverPipeline::UpdateStream(StreamConfig *config) {
  stream_enabled_ = config->view == StreamConfig::View::ORIGINAL;
  spdlog::info("DriverPipeline<{}> stream enabled: {}", inum_,
               stream_enabled_.load());
  delete config;
}

#ifndef __APPLE__
namespace {
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
}  // namespace
#endif

cv::VideoCapture DriverPipeline::GetVideoCapture() {
#ifdef __APPLE__
  cv::VideoCapture cap{0, cv::CAP_AVFOUNDATION};
#else
  GStreamerConfig gsc = pipeline_config_->gstreamer_config;

  std::string pipeline = gstreamer_pipeline(
      gsc.capture_width, gsc.capture_height, gsc.output_width,
      gsc.output_height, gsc.frame_rate, gsc.flip_mode);
  spdlog::info("{} using gstreamer pipeline: {}", *this, pipeline);

  cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
#endif
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
  for (cv::TickMeter tm;;) {
    tm.start();

    if (cancel_.load()) {
      spdlog::info("{}: stopping", *this, inum_);
      double avg = tm.getTimeSec() / tm.getCounter();
      double fps = 1.0 / avg;
      spdlog::info("{}: avg. time = {}, FPS = {}", *this, inum_, avg, fps);
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
    link.Send();
    tm.stop();
  }
}

std::string DriverPipeline::ToString() const {
  return "DriverPipeline<" + std::to_string(inum_) + ">";
}
