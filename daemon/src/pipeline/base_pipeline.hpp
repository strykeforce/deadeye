#pragma once
#include <spdlog/spdlog.h>
#include <wpi/Logger.h>
#include <atomic>
#include <map>
#include <mutex>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include "cscore.h"
#include "pipeline.hpp"
#include "pipeline_config.hpp"

namespace deadeye {

/**
 * Pipeline class definition.
 */
template <typename T>
class BasePipeline : public Pipeline {
 public:
  BasePipeline(int inum);
  virtual ~BasePipeline() {}
  void Run() override;
  void CancelTask() override { cancel_ = true; }
  void UpdateConfig(PipelineConfig config) override {
    std::lock_guard<std::mutex> lock{update_mutex_};
    config_ = config;
    update_config_ = true;
  }
  void UpdateStream(StreamConfig config) override {
    spdlog::debug("camera({}) stream sn: {}", inum_, config.sn);
  }

  // implemented in concrete pipeline classes
  //
  // cv::Mat PreProcessFrame(cv::Mat const &frame);
  //
  // void FilterContours(std::vector<std::vector<cv::Point>> const &src,
  //                     std::vector<std::vector<cv::Point>> &dest);
  //
 protected:
  virtual cv::VideoCapture GetVideoCapture() = 0;

  cv::Mat ProcessFrame(cv::Mat const &frame);

 private:
  std::atomic<bool> cancel_{false};
  bool update_config_{false};
  std::mutex update_mutex_;
  PipelineConfig config_;
  StreamConfig stream_;
  cv::Mat cvt_color_output_;
  std::vector<std::vector<cv::Point>> find_contours_input_;
  std::vector<std::vector<cv::Point>> find_contours_output_;
};

/**
 * constructor configures cscore logging.
 */
template <typename T>
BasePipeline<T>::BasePipeline(int inum) : Pipeline{inum} {
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

/**
 * Run is started in a new thread to start the pipeline.
 */
template <typename T>
void BasePipeline<T>::Run() {
  cancel_ = false;
  spdlog::info("Pipeline<{}>: starting", inum_);

  int port = 5800 + inum_;
  cs::CvSource cvsource{"cvsource", cs::VideoMode::kMJPEG, 320, 240, 30};
  cs::MjpegServer mjpegServer{"cvhttpserver", port};
  mjpegServer.SetSource(cvsource);
  spdlog::info("Pipeline<{}> listening on port {}", inum_, port);

  cv::Mat frame;
  cv::VideoCapture cap = GetVideoCapture();

  if (!cap.isOpened()) {
    spdlog::critical("Pipeline<{}>: unable to open camera({}) in {}, line {}",
                     inum_, inum_, __FILE__, __LINE__);
    throw PipelineException("unable to open camera");
  }

  PipelineConfig config;
  cv::TickMeter tm;

  // Loop until task cancelled.
  while (true) {
    tm.start();

    // Check for cancellation of this task.
    if (cancel_.load()) {
      spdlog::info("Pipeline<{}>: stopping", inum_);
      double avg = tm.getTimeSec() / tm.getCounter();
      double fps = 1.0 / avg;
      spdlog::info("Pipeline<{}>: avg. time = {}, FPS = {}", inum_, avg, fps);
      return;
    }

    // Check for new pipeline config.
    if (update_config_) {
      std::lock_guard<std::mutex> lock{update_mutex_};
      update_config_ = false;
      config = config_;
      spdlog::debug("Pipeline<{}>: config = {}", inum_, config);
    }

    // Get new frame and process it.
    cap >> frame;
    auto preview = ProcessFrame(frame);
    cvsource.PutFrame(preview);
    tm.stop();
  }
}

/**
 * ProcessFrame runs the pipeline on a captured frame. It calls into the
 * concrete pipeline implementation.
 */
template <typename T>
cv::Mat BasePipeline<T>::ProcessFrame(cv::Mat const &frame) {
  // CRTP cast to concrete pipeline implementation.
  T &impl = static_cast<T &>(*this);

  cv::Mat pre = impl.PreProcessFrame(frame);
  cv::Mat result;

  cv::resize(pre, result, cv::Size(320, 240), 0, 0, cv::INTER_AREA);
  impl.FindContours(find_contours_input_, find_contours_output_);
  return result;
}
}  // namespace deadeye
