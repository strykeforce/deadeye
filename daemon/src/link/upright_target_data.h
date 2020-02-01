#pragma once
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>

#include "link/target_data.h"

using json = nlohmann::json;

namespace deadeye {
struct UprightTargetData : public TargetData {
  static char const* kTLX;
  static char const* kTLY;
  static char const* kBRX;
  static char const* kBRY;
  static char const* kXKey;
  static char const* kYKey;

  cv::Rect bb;
  cv::Point offset;
  UprightTargetData(std::string id, int sn, bool valid, cv::Rect bb,
                    cv::Point center);

  void DrawMarkers(cv::Mat& preview) override;
  std::string Dump() const override;
  std::string ToString() const override;
};
}  // namespace deadeye
