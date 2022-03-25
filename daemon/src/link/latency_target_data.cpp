// Copyright (c) 2022 Stryke Force FRC 2767

#include "latency_target_data.h"

#include <string>

using ::deadeye::LatencyTargetData;

namespace {
constexpr std::string_view kLightsKey{"l"};
constexpr std::string_view kTimestampKey{"ts"};
}  // namespace

LatencyTargetData::LatencyTargetData(std::string_view id, int sn, bool valid,
                                     cv::Rect bb, bool lights, int64 timestamp)
    : UprightTargetData(id, sn, valid, bb),
      lights(lights),
      timestamp(timestamp) {}

std::string LatencyTargetData::Dump() const {
  json j =
      json{{TargetData::kIdKey, id},
           {TargetData::kSerialKey, serial},
           {TargetData::kValidKey, valid},
           {TargetData::kDataKey,
            {bb.tl().x, bb.tl().y, bb.br().x, bb.br().y, center.x, center.y}},
           {kLightsKey, lights},
           {kTimestampKey, timestamp}};
  return j.dump();
}
