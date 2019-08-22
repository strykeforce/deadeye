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

  /**
   * UpdateConfig handles user changes to camera config.
   */
  void UpdateConfig(PipelineConfig *config) override {
    // The config is read for every frame but updated very infrequently.
    // ProcessFrame accesses the current config throught the atomic pointer
    // pipeline_config_. To prevent deleting the config ProcessFrame is using
    // during this update, the previous config is retained and pointed to by
    // prev_pipeline_config_. It is then deleted and replaced the next time the
    // config is updated. This assumes that ProcessFrame will finish using the
    // config before this update is called again, a safe assumption since
    // updates are user initiated via the web admin UI.
    if (prev_pipeline_config_ != nullptr)
      spdlog::debug("Pipeline<{}> deleting previous config: {}", inum_,
                    *prev_pipeline_config_);

    delete prev_pipeline_config_;
    prev_pipeline_config_ = pipeline_config_.load();
    pipeline_config_.store(config);
    spdlog::debug("Pipeline<{}> new config: {}", inum_,
                  *(pipeline_config_.load()));
    if (prev_pipeline_config_ != nullptr)
      spdlog::debug("Pipeline<{}> previous config: {}", inum_,
                    *prev_pipeline_config_);
  }

  /**
   * UpdateStream handles changes to video streaming.
   */
  void UpdateStream(StreamConfig config) override { stream_ = config; }

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
  StreamConfig stream_;
  std::atomic<PipelineConfig *> pipeline_config_{nullptr};
  PipelineConfig *prev_pipeline_config_{nullptr};
  bool update_config_{false};
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

    // Get new frame and process it.
    cap >> frame;
    auto preview = ProcessFrame(frame);

    if (stream_.view != StreamConfig::View::NONE) {
      cvsource.PutFrame(preview);
    }

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

  // PipelineConfig *config = pipeline_config_.load();

  cv::Mat pre = impl.PreProcessFrame(frame);
  cv::Mat result;

  cv::resize(pre, result, cv::Size(320, 240), 0, 0, cv::INTER_AREA);
  impl.FilterContours(find_contours_input_, find_contours_output_);
  return result;
}
}  // namespace deadeye
