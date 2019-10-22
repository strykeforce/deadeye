#include "default_pipeline.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <opencv2/imgproc.hpp>

#include "config/pipeline_config.hpp"
#include "config/stream_config.hpp"
#include "link/center_target_data.hpp"

using namespace deadeye;

cv::VideoCapture DefaultPipeline::GetVideoCapture() {
#ifdef __APPLE__
  cv::VideoCapture cap{0, cv::CAP_AVFOUNDATION};
#else
  PipelineConfig *pipeline_config = pipeline_config_.load();
  GStreamerConfig gsc = pipeline_config->gstreamer_config;

  std::string pipeline = gsc.GetJetsonCSI();
  spdlog::debug("{}: {}", *this, pipeline);

  cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
#endif
  return cap;
}

cv::Mat DefaultPipeline::PreProcessFrame(cv::Mat const &frame) {
  cv::Mat result;
#ifdef __APPLE__
  // 1280 x 720
  cv::copyMakeBorder(frame, result, 120, 120, 0, 0, cv::BORDER_CONSTANT,
                     cv::Scalar(0, 0, 0));
#else
  // 320 x 180
  cv::copyMakeBorder(frame, result, 30, 30, 0, 0, cv::BORDER_CONSTANT,
                     cv::Scalar(0, 0, 0));
#endif
  return result;
}

// This filter returns the contour with the largest area.
void DefaultPipeline::FilterContours(Contours const &src, Contours &dest) {
  auto max_area_iter = std::max_element(
      src.begin(), src.end(),
      [](std::vector<cv::Point> const &a, std::vector<cv::Point> const &b) {
        return cv::contourArea(a) < cv::contourArea(b);
      });
  if (max_area_iter != src.end()) dest.push_back(*max_area_iter);

  // throw PipelineException("Test Exception");
}

std::unique_ptr<TargetData> DefaultPipeline::ProcessTarget(
    Contours const &contours) {
  return std::make_unique<CenterTargetData>(id_, 0, false, 0, 0);
}

std::string DefaultPipeline::ToString() const {
  return fmt::format("DefaultPipeline<{}>", inum_);
}
