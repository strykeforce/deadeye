#pragma once
#include <networktables/NetworkTableValue.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {
static char const* kSerialKey{"sn"};
static char const* kHsvLowKey{"hsv_low"};
static char const* kHsvHighKey{"hsv_high"};
static char const* kExposureKey{"exposure"};

struct PipelineConfig {
  int sn = 0;
  std::array<int, 3> hsv_low;
  std::array<int, 3> hsv_high;
  double exposure;

  /**
   * New is factory method to create a PipelineConfig from a NT value.
   */
  static PipelineConfig New(std::shared_ptr<nt::Value> value);
};

void to_json(json& j, const PipelineConfig& p);
void from_json(const json& j, PipelineConfig& p);

inline bool operator==(PipelineConfig const& lhs, PipelineConfig const& rhs) {
  return lhs.sn == rhs.sn && lhs.hsv_low == rhs.hsv_low &&
         lhs.hsv_high == rhs.hsv_high && lhs.exposure == rhs.exposure;
}

inline bool operator!=(PipelineConfig const& lhs, PipelineConfig const& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye

std::ostream& operator<<(std::ostream& os, deadeye::PipelineConfig const& pc);
