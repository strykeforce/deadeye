#include "default_pipeline.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <opencv2/imgproc.hpp>

#include "config/pipeline_config.hpp"
#include "config/stream_config.hpp"
#include "link/center_target_data.hpp"

using namespace deadeye;

DefaultPipeline::DefaultPipeline(int inum) : AbstractPipeline{inum} {}

bool DefaultPipeline::StartCapture() {
  if (cap_.isOpened()) return true;
#ifdef __APPLE__
  return cap_.open(0, cv::CAP_AVFOUNDATION);
#else
  safe::ReadAccess<LockablePipelineConfig> pc{pipeline_config_};
  GStreamerConfig gsc = pc->gstreamer_config;

  std::string pipeline{gsc.GetJetsonCSI()};
  spdlog::debug("{}: {}", *this, pipeline);

  return cap_.open(pipeline, cv::CAP_GSTREAMER);
#endif
}

void DefaultPipeline::StopCapture() { cap_.release(); }

bool DefaultPipeline::GrabFrame(cv::Mat &frame) {
  if (!cap_.read(frame)) return false;  // TODO: check for empty?
  // change aspect ration
  int border = frame.rows / 6;
  cv::copyMakeBorder(frame, frame, border, border, 0, 0, cv::BORDER_CONSTANT,
                     cv::Scalar(0, 0, 0));
  return true;
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

TargetDataPtr DefaultPipeline::ProcessTarget(Contours const &contours) {
  return std::make_unique<CenterTargetData>(id_, 0, false, 0, 0);
}

std::string DefaultPipeline::ToString() const {
  return fmt::format("DefaultPipeline<{}>", inum_);
}
