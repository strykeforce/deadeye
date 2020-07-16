#include "default_pipeline.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <opencv2/imgproc.hpp>

#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "link/upright_target_data.h"
#include "pipeline/pipeline_ops.h"

using namespace deadeye;

DefaultPipeline::DefaultPipeline(int inum) : AbstractPipeline{inum} {}

bool DefaultPipeline::StartCapture() {
  if (cap_.isOpened()) return true;

  auto pipeline = capture_config_.Pipeline();
  spdlog::debug("{}: {}", *this, pipeline);
  center_ = cv::Point{capture_config_.output_width / 2,
                      capture_config_.output_height / 2};
  center2f_ = static_cast<cv::Point2f>(center_);
  frame_area_ = capture_config_.output_height * capture_config_.output_width;
  return cap_.open(pipeline, cv::CAP_GSTREAMER);
}

void DefaultPipeline::StopCapture() { cap_.release(); }

bool DefaultPipeline::GrabFrame(cv::Mat &frame) { return cap_.read(frame); }

// Filter with FilterConfig or max area contour
void DefaultPipeline::FilterContours(FilterConfig const &filter,
                                     Contours const &src, Contours &dest) {
  GeometricContoursFilter(filter, src, dest);
}

TargetDataPtr DefaultPipeline::ProcessTarget(Contours const &contours) {
  return std::make_unique<TargetData>(id_, 0, false);
}

std::string DefaultPipeline::ToString() const {
  assert(pipeline_type_ != "");
  return fmt::format("DefaultPipeline<{}, {}>", id_, pipeline_type_);
}
