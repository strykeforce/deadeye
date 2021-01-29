#pragma once
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>

#include "link/target_data.h"

using json = nlohmann::json;

namespace deadeye {
using TargetList = std::vector<std::array<int, 5>>;

struct TargetListTargetData : public TargetData {
  TargetList targets;

  TargetListTargetData(const std::string id, const int sn, const bool valid,
                       const TargetList& targets);

  void DrawMarkers(cv::Mat& preview) const override;
  std::string Dump() const override;
  std::string ToString() const override;
};
}  // namespace deadeye
