#pragma once
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>

#include "link/target_data.h"

using json = nlohmann::json;

namespace deadeye {
struct MinAreaTargetData : public TargetData {
  cv::RotatedRect rect;
  cv::Point center;
  cv::Point2f corners[4];

  MinAreaTargetData(std::string_view id, int sn, bool valid, const cv::RotatedRect& rect,
                    cv::Point center);

  void DrawMarkers(cv::Mat& preview) const override;
  [[nodiscard]] std::string Dump() const override;
  [[nodiscard]] std::string ToString() const override;
};
}  // namespace deadeye
   //