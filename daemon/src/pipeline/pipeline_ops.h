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

}  // namespace deadeye