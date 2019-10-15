#include "default_pipeline.hpp"
#include <spdlog/spdlog.h>

using namespace deadeye;

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

cv::VideoCapture DefaultPipeline::GetVideoCapture() {
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

cv::Mat DefaultPipeline::PreProcessFrame(cv::Mat const &frame) {
  // need 1280 x 960
  cv::Mat result;
  cv::copyMakeBorder(frame, result, 120, 120, 0, 0, cv::BORDER_CONSTANT,
                     cv::Scalar(0, 0, 0));
  return result;
}

// This filter returns the contour with the largest area.
void DefaultPipeline::FilterContours(
    std::vector<std::vector<cv::Point>> const &src,
    std::vector<std::vector<cv::Point>> &dest) {
  auto max_area_iter = std::max_element(
      src.begin(), src.end(),
      [](std::vector<cv::Point> const &a, std::vector<cv::Point> const &b) {
        return cv::contourArea(a) < cv::contourArea(b);
      });
  if (max_area_iter != src.end()) dest.push_back(*max_area_iter);

  // throw PipelineException("Test Exception");
}

std::string DefaultPipeline::ToString() const {
  return "DefaultPipeline<" + std::to_string(inum_) + ">";
}
