// Copyright (c) 2022 Stryke Force FRC 2767

#include "latency_pipeline.h"

#include <fmt/core.h>

#include <chrono>
#include <memory>
#include <opencv2/imgproc.hpp>
#include <string>
#include <utility>

#include "link/latency_target_data.h"

using ::deadeye::LatencyPipeline;
using ::deadeye::TargetData;

[[maybe_unused]] LatencyPipeline::LatencyPipeline(int inum, std::string name)
    : AbstractPipeline{inum, std::move(name)},
      start_{std::chrono::high_resolution_clock::now()} {}

// Target is center of contour bounding box.
std::unique_ptr<TargetData> LatencyPipeline::ProcessContours(
    Contours const& contours) {
  const auto now = std::chrono::high_resolution_clock::now();
  const auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - start_)
          .count();

  if (contours.empty())
    return std::make_unique<LatencyTargetData>(
        id_, 0, false, cv::Rect{0, 0, 0, 0}, IsLedOn(), elapsed);
  const auto& contour = contours[0];
  cv::Rect bb = cv::boundingRect(contour);

  return std::make_unique<LatencyTargetData>(id_, 0, true, bb, IsLedOn(),
                                             elapsed);
}

std::string LatencyPipeline::ToString() const {
  return fmt::format("LatencyPipeline<{}>", id_);
}
