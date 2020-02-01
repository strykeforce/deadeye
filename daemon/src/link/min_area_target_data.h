#pragma once
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>

#include "link/target_data.h"

using json = nlohmann::json;

namespace deadeye {
struct MinAreaTargetData : public TargetData {
  static char const* kBLX;
  static char const* kBLY;
  static char const* kTLX;
  static char const* kTLY;
  static char const* kTRX;
  static char const* kTRY;
  static char const* kBRX;
  static char const* kBRY;
  static char const* kXKey;
  static char const* kYKey;
  static char const* kWKey;
  static char const* kHKey;
  static char const* kAngleKey;

  cv::RotatedRect rect;
  cv::Point2f offset;
  MinAreaTargetData(std::string id, int sn, bool valid, cv::RotatedRect rect,
                    cv::Point2f center);

  void DrawMarkers(cv::Mat& preview) override;
  std::string Dump() const override;
  std::string ToString() const override;
};
}  // namespace deadeye
   //