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
#include "link.hpp"
#include "pipeline.hpp"
#include "pipeline_config.hpp"

#define PREVIEW_WIDTH 320
#define PREVIEW_HEIGHT 240

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
    delete prev_pipeline_config_;
    prev_pipeline_config_ = pipeline_config_.load();
    pipeline_config_.store(config);
    spdlog::debug("Pipeline<{}> new config: {}", inum_,
                  *(pipeline_config_.load()));
  }

  /**
   * UpdateStream handles changes to video streaming.
   */
  void UpdateStream(StreamConfig *config) override {
    // Same algorithm as UpdateConfig above.
    delete prev_stream_config_;
    prev_stream_config_ = stream_config_.load();
    stream_config_.store(config);
    spdlog::debug("Pipeline<{}> new config: {}", inum_,
                  *(stream_config_.load()));
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

  void ProcessFrame(cv::Mat const &frame);

 private:
  // config variables
  std::atomic<bool> cancel_{false};
  std::atomic<StreamConfig *> stream_config_{nullptr};
  StreamConfig *prev_stream_config_{nullptr};
  std::atomic<PipelineConfig *> pipeline_config_{nullptr};
  PipelineConfig *prev_pipeline_config_{nullptr};
  void LogTickMeter(cv::TickMeter tm);

  // pipeline variables
  // cv::Mat preprocess_output_;
  // cv::Mat cvt_color_output_;
  // cv::Mat hsv_threshold_output_;
  // std::vector<std::vector<cv::Point>> find_contours_output_;
  // std::vector<std::vector<cv::Point>> filter_contours_output_;
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

  cv::Mat frame;
  cv::VideoCapture cap = GetVideoCapture();

  if (!cap.isOpened()) {
    spdlog::critical("Pipeline<{}>: unable to open camera({}) in {}, line {}",
                     inum_, inum_, __FILE__, __LINE__);
    throw PipelineException("unable to open camera");
  }

  // Set up streaming. CScore streaming will hang on connection if too many
  // connections are attempted, current workaround is for user to  disable and
  // reenable the stream to reset.
  cs::CvSource cvsource{"cvsource", cs::VideoMode::kMJPEG, PREVIEW_WIDTH,
                        PREVIEW_HEIGHT, 30};
  cs::MjpegServer mjpegServer{"cvhttpserver", 5800 + inum_};
  mjpegServer.SetSource(cvsource);
  spdlog::info("Pipeline<{}> streaming on port {}", inum_,
               mjpegServer.GetPort());

  Link link{inum_};

  // Loop until task cancelled.
  for (cv::TickMeter tm;;) {
    tm.start();

    // Check for cancellation of this task.
    if (cancel_.load()) {
      LogTickMeter(tm);
      return;
    }

    // Get new frame and process it.
    cap >> frame;

    PipelineConfig *config = pipeline_config_.load();
    T &impl = static_cast<T &>(*this);
    cv::Mat preprocess_output = impl.PreProcessFrame(frame);

    cv::Mat hsv_threshold_output;
    cv::cvtColor(preprocess_output, hsv_threshold_output, cv::COLOR_BGR2HSV);
    cv::inRange(hsv_threshold_output,
                cv::Scalar(config->hue[0], config->sat[0], config->val[0]),
                cv::Scalar(config->hue[1], config->sat[1], config->val[1]),
                hsv_threshold_output);

    std::vector<std::vector<cv::Point>> find_contours_output;
    cv::findContours(hsv_threshold_output, find_contours_output,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // spdlog::debug("Contours found: {}", find_contours_output.size());

    std::vector<std::vector<cv::Point>> filter_contours_output;
    impl.FilterContours(find_contours_output, filter_contours_output);

    StreamConfig *stream = stream_config_.load();
    if (stream->view != StreamConfig::View::NONE ||
        stream->contour != StreamConfig::Contour::NONE) {
      cv::Mat preview;
      switch (stream->view) {
        case StreamConfig::View::NONE:
          if (stream->contour != StreamConfig::Contour::NONE) {
            preview = cv::Mat::zeros(preprocess_output.size(), CV_8UC3);
          }
          break;
        case StreamConfig::View::ORIGINAL:
          if (stream->contour != StreamConfig::Contour::NONE) {
            cv::cvtColor(preprocess_output, preview, cv::COLOR_BGR2GRAY);
            cv::cvtColor(preview, preview, cv::COLOR_GRAY2BGR);
            break;
          }
          preview = preprocess_output;
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
          break;
        case StreamConfig::Contour::ALL:
          cv::drawContours(preview, find_contours_output, -1,
                           cv::Scalar(255, 0, 240), 2);
          break;
      }

      cv::resize(preview, preview, cv::Size(PREVIEW_WIDTH, PREVIEW_HEIGHT), 0,
                 0, cv::INTER_AREA);
      cvsource.PutFrame(preview);
    }

    link.Send();

    tm.stop();
  }
}

template <typename T>
void BasePipeline<T>::LogTickMeter(cv::TickMeter tm) {
  spdlog::info("Pipeline<{}>: stopping", inum_);
  double avg = tm.getTimeSec() / tm.getCounter();
  double fps = 1.0 / avg;
  spdlog::info("Pipeline<{}>: avg. time = {}, FPS = {}", inum_, avg, fps);
}

}  // namespace deadeye
