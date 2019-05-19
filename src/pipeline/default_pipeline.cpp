#include <spdlog/spdlog.h>
#include "default_pipeline.hpp"

using namespace deadeye;

void DefaultPipeline::FindContours(
    std::vector<std::vector<cv::Point>> const &src,
    std::vector<std::vector<cv::Point>> &dest) {
  // throw PipelineException("Test Exception");
}

cv::VideoCapture DefaultPipeline::GetVideoCapture() {
  spdlog::debug("{}", __PRETTY_FUNCTION__);
  cv::VideoCapture cap;
  cap.open(0, cv::CAP_AVFOUNDATION);
  return cap;
}
