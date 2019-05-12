#include <spdlog/spdlog.h>
#include <opencv2/opencv.hpp>
#include "cscore.h"
#include "pipeline.hpp"

using namespace deadeye;

void Pipeline::Quit() { quit_.store(true); }

void Pipeline::Run() {
  quit_.store(false);
  spdlog::info("{} starting", *this);

  cs::CvSource cvsource{"cvsource", cs::VideoMode::kMJPEG, 320, 180, 30};
  cs::MjpegServer cvMjpegServer{"cvhttpserver", 8083};
  cvMjpegServer.SetSource(cvsource);

  cv::Mat test;
  cv::Mat flip;
  spdlog::info("{} initializing camera", *this);
  cv::VideoCapture cap{0};

  spdlog::debug("{} checking camera is opened", *this);
  if (!cap.isOpened()) {
    spdlog::critical("{} camera not opened", *this);
    return;
  }

  while (true) {
    if (quit_.load()) {
      spdlog::info("{} stopping", *this);
      return;
    }

    cap >> test;
    cv::resize(test, flip, cv::Size(320, 180));
    cvsource.PutFrame(flip);
  }
}
