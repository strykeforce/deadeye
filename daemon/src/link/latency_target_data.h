// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <string>

#include "upright_target_data.h"

namespace deadeye {
struct LatencyTargetData : public UprightTargetData {
  bool lights;
  int64 timestamp;

  LatencyTargetData(std::string_view id, int sn, bool valid, cv::Rect bb,
                    bool lights, int64 timestamp);

  [[nodiscard]] std::string Dump() const override;
};
}  // namespace deadeye
