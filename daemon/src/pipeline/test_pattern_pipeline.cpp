#include "test_pattern_pipeline.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <opencv2/imgproc.hpp>

#include "link/center_target_data.hpp"

using namespace deadeye;

TestPatternPipeline::TestPatternPipeline(int inum) : AbstractPipeline{inum} {}

bool TestPatternPipeline::StartCapture() {
  if (cap_.isOpened()) return true;
  std::string pipeline{
      "videotestsrc ! video/x-raw, width=640, "
      "height=480, "
      "framerate=90/1 ! videoconvert ! video/x-raw, format=(string)BGR ! "
      "appsink"};
  return cap_.open(pipeline, cv::CAP_GSTREAMER);
}

void TestPatternPipeline::StopCapture() { cap_.release(); }

bool TestPatternPipeline::GrabFrame(cv::Mat &frame) {
  if (!cap_.read(frame)) return false;  // TODO: check for empty?
  return true;
}

// This filter returns the contour with the largest area.
void TestPatternPipeline::FilterContours(Contours const &src, Contours &dest) {
  auto max_area_iter = std::max_element(
      src.begin(), src.end(),
      [](std::vector<cv::Point> const &a, std::vector<cv::Point> const &b) {
        return cv::contourArea(a) < cv::contourArea(b);
      });
  if (max_area_iter != src.end()) dest.push_back(*max_area_iter);

  // throw PipelineException("Test Exception");
}

TargetDataPtr TestPatternPipeline::ProcessTarget(Contours const &contours) {
  if (contours.size() == 0)
    return std::make_unique<CenterTargetData>(id_, 0, false, 0, 0);
  auto contour = contours[0];
  auto rect = cv::boundingRect(contour);
  int x = rect.x + (rect.width / 2);
  int y = rect.y + (rect.height / 2);

  return std::make_unique<CenterTargetData>(id_, 0, true, x, y);
}

std::string TestPatternPipeline::ToString() const {
  return fmt::format("TestPatternPipeline<{}>", inum_);
}
