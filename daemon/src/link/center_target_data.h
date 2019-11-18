#pragma once
#include <nlohmann/json.hpp>

#include "link/target_data.h"

using json = nlohmann::json;

namespace deadeye {
struct CenterTargetData : public TargetData {
  static char const* kXKey;
  static char const* kYKey;

  // x & y are offsets from frame center point
  int x;
  int y;

  CenterTargetData(std::string id, int sn, bool valid, int x, int y);

  void DrawMarkers(cv::Mat& preview) override;
  std::string Dump() const override;
  std::string ToString() const override;
};

void to_json(json& j, CenterTargetData const& td);
void from_json(json const& j, CenterTargetData& td);

}  // namespace deadeye
