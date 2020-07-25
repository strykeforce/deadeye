#include "upright_rect_pipeline.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <opencv2/imgproc.hpp>

// #include "config/pipeline_config.h"
// #include "config/stream_config.h"
#include "link/upright_target_data.h"

using namespace deadeye;

UprightRectPipeline::UprightRectPipeline(int inum) : AbstractPipeline{inum} {}

void UprightRectPipeline::Configure(const CaptureConfig& config) {
  center_ = config.Size() / 2;
  capture_type_ = config.PipelineType();
}

// Target is center of contour bounding box.
TargetDataPtr UprightRectPipeline::ProcessTarget(Contours const& contours) {
  if (contours.empty())
    return std::make_unique<UprightTargetData>(id_, 0, false,
                                               cv::Rect{0, 0, 0, 0}, center_);
  auto contour = contours[0];
  cv::Rect bb = cv::boundingRect(contour);
  return std::make_unique<UprightTargetData>(id_, 0, true, bb, center_);
}

std::string UprightRectPipeline::ToString() const {
  return fmt::format("UprightRectPipeline<{}, {}>", id_, capture_type_);
}
