#pragma once

#include <opencv2/imgproc.hpp>

namespace deadeye {
inline void MaskFrame(cv::Mat const& frame, cv::Mat& output,
                      cv::Scalar const& low, cv::Scalar const& high) {
  cv::cvtColor(frame, output, cv::COLOR_BGR2HSV);
  cv::inRange(output, (low), (high), output);
}

using Contours = std::vector<std::vector<cv::Point>>;

inline void FindContours(cv::Mat const& mask, Contours& contours) {
  contours.clear();
  cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
}

/**
 * Filters according to filter; dest is copy of src if filters not enabled;
 */
inline void GeometricContoursFilter(FilterConfig const& filter,
                                    Contours const& src, Contours& dest) {
  if (!filter.enabled) {
    dest = src;
    return;
  }

  dest.clear();

#ifndef NDEBUG
  if (filter.area_enabled) assert(filter.frame_area > 0);
#endif

  for (auto const& contour : src) {
    // set these to true if filter is skipped, false otherwise
    bool area_ok = !filter.area_enabled;
    bool fullness_ok = !filter.fullness_enabled;
    bool aspect_ok = !filter.aspect_enabled;

    double area = cv::contourArea(contour);
    cv::Rect bb = cv::boundingRect(contour);
    double bb_area = static_cast<double>(bb.area());

    if (filter.area_enabled) {
      double ratio = bb_area / filter.frame_area;
      area_ok = ratio >= filter.area[0] && ratio <= filter.area[1];
      // spdlog::debug("bb = {}, frame = {}, ratio = {}", bb.area(),
      //               filter.frame_area, ratio);
    }

    if (filter.fullness_enabled) {
      std::vector<cv::Point> hull;
      cv::convexHull(contour, hull);
      double hull_area = cv::contourArea(hull);
      double fullness = area / hull_area;
      fullness_ok =
          fullness >= filter.fullness[0] && fullness <= filter.fullness[1];
      // spdlog::debug("fullness = {}", fullness);
    }

    if (filter.aspect_enabled) {
      double aspect = static_cast<double>(bb.width) / bb.height;
      aspect_ok = aspect >= filter.aspect[0] && aspect <= filter.aspect[1];
      // spdlog::debug("aspect = {}", aspect);
    }

    if (area_ok && fullness_ok && aspect_ok) {
      dest.push_back(contour);
    }
  }
}

}  // namespace deadeye