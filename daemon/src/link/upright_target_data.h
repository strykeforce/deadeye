#pragma once
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>

#include "link/target_data.h"

using json = nlohmann::json;

namespace deadeye {
struct UprightTargetData : public TargetData {
  cv::Rect bb;
  cv::Point center;
  UprightTargetData(std::string id, int sn, bool valid, cv::Rect bb);

  void DrawMarkers(cv::Mat& preview) const override;
  std::string Dump() const override;
  std::string ToString() const override;
};
}  // namespace deadeye
