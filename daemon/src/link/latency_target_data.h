#pragma once

#include "upright_target_data.h"

namespace deadeye {
struct LatencyTargetData : public UprightTargetData {
  bool lights;
  long long timestamp;

  LatencyTargetData(std::string_view id, int sn, bool valid, cv::Rect bb,
                    bool lights, long long timestamp);

  [[nodiscard]] std::string Dump() const override;
};
}  // namespace deadeye
