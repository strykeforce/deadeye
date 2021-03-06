#include "pipeline/target_list_pipeline.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <opencv2/imgproc.hpp>

#include "link/target_list_target_data.h"

using namespace deadeye;

TargetListPipeline::TargetListPipeline(int inum, std::string name)
    : AbstractPipeline{inum, name} {}

void TargetListPipeline::Configure(const CaptureConfig& config) {
  capture_type_ = config.PipelineType();
}

// Target is center of contour bounding box.
std::unique_ptr<TargetData> TargetListPipeline::ProcessContours(
    Contours const& contours) {
  TargetList targets;

  for (const auto& contour : contours) {
    cv::Rect bb = cv::boundingRect(contour);
    int area = std::round(cv::contourArea(contour));
    targets.push_back({bb.x, bb.y, bb.width, bb.height, area});
  }

  return std::make_unique<TargetListTargetData>(id_, 0, !contours.empty(),
                                                targets);
}

std::string TargetListPipeline::ToString() const {
  return fmt::format("TargetListPipeline<{}, {}>", id_, capture_type_);
}
