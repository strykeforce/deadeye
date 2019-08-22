#include "default_pipeline.hpp"
#include <spdlog/spdlog.h>

using namespace deadeye;

void DefaultPipeline::FilterContours(
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

cv::Mat DefaultPipeline::PreProcessFrame(cv::Mat const &frame) {
  // need 1280 x 960
  cv::Mat result;
  cv::copyMakeBorder(frame, result, 120, 120, 0, 0, cv::BORDER_CONSTANT,
                     cv::Scalar(0, 0, 0));
  return result;
}
