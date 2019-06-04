#include <spdlog/spdlog.h>
#include "test_pattern_pipeline.hpp"

using namespace deadeye;

cv::VideoCapture TestPatternPipeline::GetVideoCapture() {
  spdlog::debug("{}", __PRETTY_FUNCTION__);
  cv::VideoCapture cap;
  std::string s{
      "videotestsrc ! video/x-raw, width=640, "
      "height=480, "
      "framerate=90/1 ! videoconvert ! appsink"};
  cap.open(s, cv::CAP_GSTREAMER);
  return cap;
}
void TestPatternPipeline::FindContours(
    std::vector<std::vector<cv::Point>> const &src,
    std::vector<std::vector<cv::Point>> &dest) {
  // throw PipelineException("Test Exception");
}
