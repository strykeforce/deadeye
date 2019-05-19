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
    update_sn_++;
  }

 protected:
  virtual cv::VideoCapture GetVideoCapture() {
    spdlog::debug("{}", __PRETTY_FUNCTION__);
    cv::VideoCapture cap;
    std::string s{
        "videotestsrc ! video/x-raw, width=640, "
        "height=360, "
        "framerate=90/1 ! videoconvert ! appsink"};
    cap.open(s, cv::CAP_GSTREAMER);
    return cap;
  }
  void ProcessFrame(cv::Mat const &frame);

  // implemented in concrete pipeline classes
  // void FilterContours(std::vector<std::vector<cv::Point>> const &src,
  //                     std::vector<std::vector<cv::Point>> &dest);

 private:
  std::atomic<bool> cancel_{false};
  std::atomic<int> update_sn_{1};
  std::mutex update_mutex_;
  PipelineConfig config_;
  cv::Mat cvt_color_output_;
  std::vector<std::vector<cv::Point>> find_contours_input_;
  std::vector<std::vector<cv::Point>> find_contours_output_;
};

/**
 * constructor configures cscore logging.
 */
template <typename t>
BasePipeline<t>::BasePipeline(int inum) : Pipeline{inum} {
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

  cs::CvSource cvsource{"cvsource", cs::VideoMode::kMJPEG, 320, 180, 30};
  cs::MjpegServer cvMjpegServer{"cvhttpserver", 8083};
  cvMjpegServer.SetSource(cvsource);

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
      spdlog::debug("Pipeline<{}>: avg. time = {}, FPS = {}", inum_, avg, fps);
      return;
    }

    // Check for new pipeline config. Atomic int is incremented in
    // UpdateConfig() above and compared here with last update. If updated
    // again during this, catch new update next time around.
    int update_sn = update_sn_.load();
    if (update_sn > config.sn) {
      std::lock_guard<std::mutex> lock{update_mutex_};
      config = config_;
      config.sn = update_sn;
      spdlog::debug("Pipeline<{}>: config sn = {}", inum_, config.sn);
    }

    // Get new frame and process it.
    cap >> frame;
    ProcessFrame(frame);
    cvsource.PutFrame(cvt_color_output_);
    tm.stop();
  }
}

/**
 * ProcessFrame runs the pipeline on a captured frame. It calls into the
 * concrete pipeline implementation.
 */
template <typename T>
void BasePipeline<T>::ProcessFrame(cv::Mat const &frame) {
  // CRTP cast to concrete pipeline implementation.
  T &impl = static_cast<T &>(*this);

  cv::resize(frame, cvt_color_output_, cv::Size(320, 180));
  impl.FindContours(find_contours_input_, find_contours_output_);
}
}  // namespace deadeye
