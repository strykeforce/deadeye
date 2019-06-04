#pragma once
#include <networktables/NetworkTableValue.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {

struct PipelineConfig {
  static char const* kSerialKey;
  static char const* kHsvHueKey;
  static char const* kHsvSatKey;
  static char const* kHsvValKey;
  static char const* kExposureKey;

  int sn = 0;
  std::array<int, 2> hue;
  std::array<int, 2> sat;
  std::array<int, 2> val;
  double exposure;

  /**
   * New is factory method to create a PipelineConfig from a NT value.
   */
  static PipelineConfig New(std::shared_ptr<nt::Value> value);
};

void to_json(json& j, const PipelineConfig& p);
void from_json(const json& j, PipelineConfig& p);

inline bool operator==(PipelineConfig const& lhs, PipelineConfig const& rhs) {
  return lhs.sn == rhs.sn && lhs.hue == rhs.hue && lhs.sat == rhs.sat &&
         lhs.val == rhs.val && lhs.exposure == rhs.exposure;
}

inline bool operator!=(PipelineConfig const& lhs, PipelineConfig const& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye

std::ostream& operator<<(std::ostream& os, deadeye::PipelineConfig const& pc);
