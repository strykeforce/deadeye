#include <spdlog/spdlog.h>
#include <opencv2/opencv.hpp>
#include "cscore.h"
#include "pipeline.hpp"

using namespace deadeye;

void Pipeline::Quit() { quit_.store(true); }

void Pipeline::Run() {
  std::stringstream ss;
  ss << std::this_thread::get_id();

  spdlog::info("Pipeline<{}> ({}) starting", inum_, ss.str());
  quit_.store(false);

  cs::CvSource cvsource{"cvsource", cs::VideoMode::kMJPEG, 320, 180, 30};
  cs::MjpegServer cvMjpegServer{"cvhttpserver", 8083};
  cvMjpegServer.SetSource(cvsource);

  cv::Mat test;
  cv::Mat flip;
  spdlog::info("Pipeline<{}> initializing camera", inum_);
  cv::VideoCapture cap{0};

  spdlog::debug("Pipeline<{}> checking camera is opened", inum_);
  if (!cap.isOpened()) {
    spdlog::critical("Pipeline<{}> camera not opened", inum_);
    return;
  }

  while (true) {
    if (quit_.load()) {
      spdlog::info("Pipeline<{}> ({}) stopping", inum_, ss.str());
      return;
    }

    cap >> test;
    cv::resize(test, flip, cv::Size(320, 180));
    cvsource.PutFrame(flip);
  }
}
