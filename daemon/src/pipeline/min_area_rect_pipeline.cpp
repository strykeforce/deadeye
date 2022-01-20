#include "min_area_rect_pipeline.h"

#include <fmt/core.h>

#include <opencv2/imgproc.hpp>
#include <utility>

#include "link/min_area_target_data.h"

using namespace deadeye;

[[maybe_unused]] MinAreaRectPipeline::MinAreaRectPipeline(int inum, std::string name)
    : AbstractPipeline{inum, std::move(name)} {}

void MinAreaRectPipeline::Configure(const CaptureConfig& config) {
  center2f_ = static_cast<cv::Point2f>(config.Size() / 2);
  capture_type_ = config.PipelineType();
}

// Target is center of contour bounding box.
std::unique_ptr<TargetData> MinAreaRectPipeline::ProcessContours(
    Contours const& contours) {
  if (contours.empty())
    return std::make_unique<MinAreaTargetData>(id_, 0, false, cv::RotatedRect(),
                                               center2f_);
  auto contour = contours[0];
  cv::RotatedRect rect = cv::minAreaRect(contour);
  return std::make_unique<MinAreaTargetData>(id_, 0, true, rect, center2f_);
}

std::string MinAreaRectPipeline::ToString() const {
  return fmt::format("MinAreaRectPipeline<{}, {}>", id_, capture_type_);
}
