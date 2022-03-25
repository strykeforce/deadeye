#include "upright_rect_pipeline.h"

#include <fmt/core.h>

#include <opencv2/imgproc.hpp>
#include <utility>

#include "link/upright_target_data.h"

using namespace deadeye;

[[maybe_unused]] UprightRectPipeline::UprightRectPipeline(int inum,
                                                          std::string name)
    : AbstractPipeline{inum, std::move(name)} {}

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
  return fmt::format("UprightRectPipeline<{}>", id_);
}
