#pragma once
#include <spdlog/spdlog.h>
#include <wpi/Logger.h>
#include <atomic>
#include <map>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include "cscore.h"
#include "pipeline.hpp"

namespace deadeye {

class PipelineException : public std::exception {
  char const *what_;

 public:
  PipelineException(char const *what) : what_{what} {}
  const char *what() const throw() { return what_; }
};

/**
 * Pipeline class definition.
 */
template <typename T>
class Pipeline {
 public:
  Pipeline(int inum);
  virtual ~Pipeline() {}

  void Quit() { quit_ = true; }
  void Run();

 protected:
  void ProcessFrame(cv::Mat const &frame);

  // implemented in concrete pipeline classes
  // void FilterContours(std::vector<std::vector<cv::Point>> const &src,
  //                     std::vector<std::vector<cv::Point>> &dest);

 private:
  int inum_;
  std::atomic<bool> quit_{false};
  cv::Mat cvt_color_output_;
  std::vector<std::vector<cv::Point>> find_contours_input_;
  std::vector<std::vector<cv::Point>> find_contours_output_;
};

/**
 * constructor configures cscore logging.
 */
template <typename t>
Pipeline<t>::Pipeline(int inum) : inum_(inum) {
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
void Pipeline<T>::Run() {
  quit_ = false;
  spdlog::info("Pipeline<{}>: starting", inum_);

  cs::CvSource cvsource{"cvsource", cs::VideoMode::kMJPEG, 320, 180, 30};
  cs::MjpegServer cvMjpegServer{"cvhttpserver", 8083};
  cvMjpegServer.SetSource(cvsource);

  cv::Mat frame;
  cv::VideoCapture cap;

  if (!cap.open(inum_)) {
    spdlog::critical("Pipeline<{}>: unable to open camera({}) in {}, line {}",
                     inum_, inum_, __FILE__, __LINE__);
    throw PipelineException("unable to open camera");
  }

  // loop until told to quit
  while (true) {
    if (quit_.load()) {
      spdlog::info("Pipeline<{}>: stopping", inum_);
      return;
    }

    cap >> frame;
    ProcessFrame(frame);
    cvsource.PutFrame(cvt_color_output_);
  }
}

/**
 * ProcessFrame runs the pipeline on a captured frame. It calls into the
 * concrete pipeline implementation.
 */
template <typename T>
void Pipeline<T>::ProcessFrame(cv::Mat const &frame) {
  // CRTP cast to concrete pipeline implementation.
  T &impl = static_cast<T &>(*this);

  cv::resize(frame, cvt_color_output_, cv::Size(320, 180));
  impl.FindContours(find_contours_input_, find_contours_output_);
}
}  // namespace deadeye
