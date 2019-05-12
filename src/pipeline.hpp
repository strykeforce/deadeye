#pragma once
#include <spdlog/spdlog.h>
#include <atomic>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include "cscore.h"
#include "pipeline.hpp"

namespace deadeye {

/**
 * Pipeline class definition.
 */
template <typename T>
class Pipeline {
 public:
  Pipeline(int inum) : inum_(inum) {}
  virtual ~Pipeline() {}

  void Quit() { quit_.store(true); }
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
 * Run is started in a new thread to start the pipeline.
 */
template <typename T>
void Pipeline<T>::Run() {
  quit_.store(false);
  spdlog::info("Pipeline<{}>: starting", inum_);

  cs::CvSource cvsource{"cvsource", cs::VideoMode::kMJPEG, 320, 180, 30};
  cs::MjpegServer cvMjpegServer{"cvhttpserver", 8083};
  cvMjpegServer.SetSource(cvsource);

  cv::Mat frame;
  cv::VideoCapture cap{0};

  if (!cap.isOpened()) {
    spdlog::critical("Pipeline<{}>: camera not opened in {}, line {}", inum_,
                     __FILE__, __LINE__);
    return;
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
