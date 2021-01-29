#include "upright_rect_pipeline.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <opencv2/imgproc.hpp>

// #include "config/pipeline_config.h"
// #include "config/stream_config.h"
#include "link/upright_target_data.h"

using namespace deadeye;

UprightRectPipeline::UprightRectPipeline(int inum, std::string name)
    : AbstractPipeline{inum, name} {}

void UprightRectPipeline::Configure(const CaptureConfig& config) {
  capture_type_ = config.PipelineType();
}

// Target is center of contour bounding box.
std::unique_ptr<TargetData> UprightRectPipeline::ProcessContours(
    Contours const& contours) {
  if (contours.empty())
    return std::make_unique<UprightTargetData>(id_, 0, false,
                                               cv::Rect{0, 0, 0, 0});
  auto contour = contours[0];
  cv::Rect bb = cv::boundingRect(contour);
  return std::make_unique<UprightTargetData>(id_, 0, true, bb);
}

std::string UprightRectPipeline::ToString() const {
  return fmt::format("UprightRectPipeline<{}, {}>", id_, capture_type_);
}
