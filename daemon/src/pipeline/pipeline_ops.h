// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <algorithm>
#include <opencv2/imgproc.hpp>
#include <utility>
#include <vector>

namespace deadeye {
inline void MaskFrame(const cv::Mat& frame, cv::Mat& output,
                      const cv::Scalar& low, const cv::Scalar& high) {
  cv::cvtColor(frame, output, cv::COLOR_BGR2HSV);
  cv::inRange(output, (low), (high), output);
}

using Contours = std::vector<std::vector<cv::Point>>;

inline void FindContours(const cv::Mat& mask, Contours& contours) {
  contours.clear();
  cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
}

/**
 * Filters according to filter; dest is copy of src if filters not enabled;
 */
inline void GeometricContoursFilter(const FilterConfig& filter,
                                    const Contours& src, Contours& dest) {
#ifndef NDEBUG
  if (filter.IsAreaEnabled()) assert(filter.frame_area > 0);
#endif

  dest.clear();
  // temporarily zip together contours and areas for sorting contours by area
  // after filtering
  std::vector<std::pair<std::vector<cv::Point>, double>> zipped;

  for (const auto& contour : src) {
    // set these to true if filter is skipped, false otherwise
    bool area_ok{true};
    bool solidity_ok{true};
    bool aspect_ok{true};

    double area = cv::contourArea(contour);
    cv::Rect bb = cv::boundingRect(contour);

    if (filter.IsAreaEnabled()) {
      auto bb_area = static_cast<double>(bb.area());
      double ratio = bb_area / filter.frame_area;
      area_ok = ratio >= filter.area[0] && ratio <= filter.area[1];
      // spdlog::debug("bb = {}, frame = {}, ratio = {}", bb.area(),
      //               filter.frame_area, ratio);
    }

    if (filter.IsSolidityEnabled()) {
      std::vector<cv::Point> hull;
      cv::convexHull(contour, hull);
      double hull_area = cv::contourArea(hull);
      double solidity = area / hull_area;
      solidity_ok =
          solidity >= filter.solidity[0] && solidity <= filter.solidity[1];
      // spdlog::debug("solidity = {}", solidity);
    }

    if (filter.IsAspectEnabled()) {
      double aspect = static_cast<double>(bb.width) / bb.height;
      aspect_ok = aspect >= filter.aspect[0] && aspect <= filter.aspect[1];
      // spdlog::debug("aspect = {}", aspect);
    }

    if (area_ok && solidity_ok && aspect_ok) {
      zipped.emplace_back(contour, area);
    }
  }

  std::sort(zipped.begin(), zipped.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

  for (const auto& p : zipped) dest.push_back(p.first);
}

}  // namespace deadeye
