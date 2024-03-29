// Copyright (c) 2022 Stryke Force FRC 2767

#include "min_area_target_data.h"

#include <fmt/core.h>

#include <opencv2/imgproc.hpp>
#include <string>
#include <utility>

using ::deadeye::MinAreaTargetData;
using json = nlohmann::json;

MinAreaTargetData::MinAreaTargetData(std::string_view id, int sn, bool valid,
                                     const cv::RotatedRect& rect,
                                     cv::Point center)
    : TargetData{id, sn, valid}, rect(rect), center(center) {
  rect.points(corners);
}

void MinAreaTargetData::DrawMarkers(cv::Mat& preview) const {
  cv::Point frame_center{preview.cols / 2, preview.rows / 2};
  cv::Point2f target = rect.center;
  cv::drawMarker(preview, frame_center, cv::Scalar::all(255),
                 cv::MARKER_TILTED_CROSS);
  cv::drawMarker(preview, target, cv::Scalar::all(255));

  for (const auto& corner : corners)
    cv::circle(preview, corner, 3, cv::Scalar(0, 0, 255), cv::FILLED,
               cv::LINE_AA);
  // cv::rectangle(preview, bb, cv::Scalar(255, 0, 0));
}

std::string MinAreaTargetData::Dump() const {
  json j = json{
      {TargetData::kIdKey, id},
      {TargetData::kSerialKey, serial},
      {TargetData::kValidKey, valid},
      {TargetData::kDataKey,
       {
           rect.angle,
           rect.center.x,
           rect.center.y,
           rect.size.height,
           rect.size.width,
           corners[0].x,
           corners[0].y,
           corners[1].x,
           corners[1].y,
           corners[2].x,
           corners[2].y,
           corners[3].x,
           corners[3].y,
       }},
  };
  return j.dump();
}

std::string MinAreaTargetData::ToString() const {
  return fmt::format(
      "id={} sn={} val={} bl=({:.1f},{:.1f}) tl=({:.1f},{:.1f}) "
      "tr=({:.1f},{:.1f}) br=({:.1f},{:.1f}) "
      "ctr=({},{}) w={:.1f} h={:.1f} a={:.1f}",
      id, serial, valid, corners[0].x, corners[0].y, corners[1].x, corners[1].y,
      corners[2].x, corners[2].y, corners[3].x, corners[3].y, center.x,
      center.y, rect.size.width, rect.size.height, rect.angle);
}
