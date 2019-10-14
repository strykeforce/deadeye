#include "test_pattern_pipeline.hpp"
#include <spdlog/spdlog.h>

using namespace deadeye;

cv::VideoCapture TestPatternPipeline::GetVideoCapture() {
  spdlog::debug("{}", __PRETTY_FUNCTION__);
  cv::VideoCapture cap;
  std::string s{
      "videotestsrc ! video/x-raw, width=640, "
      "height=480, "
      "framerate=90/1 ! videoconvert ! video/x-raw, format=(string)BGR ! "
      "appsink"};
  cap.open(s, cv::CAP_GSTREAMER);
  return cap;
}

// This filter returns the contour with the largest area.
void TestPatternPipeline::FilterContours(
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
