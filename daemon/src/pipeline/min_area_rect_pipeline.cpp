#include "min_area_rect_pipeline.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <opencv2/imgproc.hpp>

#include "link/min_area_target_data.h"

using namespace deadeye;

MinAreaRectPipeline::MinAreaRectPipeline(int inum) : AbstractPipeline{inum} {}

void MinAreaRectPipeline::Configure(const CaptureConfig& config) {
  center2f_ = static_cast<cv::Point2f>(config.OutputSize() / 2);
}

// Target is center of contour bounding box.
TargetDataPtr MinAreaRectPipeline::ProcessTarget(Contours const& contours) {
  if (contours.size() == 0)
    return std::make_unique<MinAreaTargetData>(id_, 0, false, cv::RotatedRect(),
                                               center2f_);
  auto contour = contours[0];
  cv::RotatedRect rect = cv::minAreaRect(contour);
  return std::make_unique<MinAreaTargetData>(id_, 0, true, rect, center2f_);
}

std::string MinAreaRectPipeline::ToString() const {
  assert(pipeline_type_ != "");
  return fmt::format("MinAreaRectPipeline<{}, {}>", id_, pipeline_type_);
}
