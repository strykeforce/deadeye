#pragma once
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>

#include "link/target_data.h"

using json = nlohmann::json;

namespace deadeye {
struct UprightTargetData : public TargetData {
  static const char* kTLX;
  static const char* kTLY;
  static const char* kBRX;
  static const char* kBRY;
  static const char* kXKey;
  static const char* kYKey;

  cv::Rect bb;
  cv::Point offset;
  UprightTargetData(std::string id, int sn, bool valid, cv::Rect bb,
                    cv::Point center);

  void DrawMarkers(cv::Mat& preview) const override;
  std::string Dump() const override;
  std::string ToString() const override;
};
}  // namespace deadeye
