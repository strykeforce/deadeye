#pragma once
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>

#include "link/target_data.h"

using json = nlohmann::json;

namespace deadeye {
struct MinAreaTargetData : public TargetData {
  /*
  static const char* kBLX;
  static const char* kBLY;
  static const char* kTLX;
  static const char* kTLY;
  static const char* kTRX;
  static const char* kTRY;
  static const char* kBRX;
  static const char* kBRY;
  static const char* kXKey;
  static const char* kYKey;
  static const char* kWKey;
  static const char* kHKey;
  static const char* kAngleKey;
  */

  cv::RotatedRect rect;
  cv::Point center;
  cv::Point2f corners[4];

  MinAreaTargetData(std::string id, int sn, bool valid, cv::RotatedRect rect,
                    cv::Point center);

  void DrawMarkers(cv::Mat& preview) const override;
  std::string Dump() const override;
  std::string ToString() const override;
};
}  // namespace deadeye
   //