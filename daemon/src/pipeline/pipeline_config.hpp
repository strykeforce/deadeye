#pragma once
#include <networktables/NetworkTableValue.h>
#include <spdlog/fmt/ostr.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {

struct PipelineConfig {
  static char const* kSerialKey;
  static char const* kHsvHueKey;
  static char const* kHsvSatKey;
  static char const* kHsvValKey;
  static char const* kExposureKey;

  using hsv_t = std::array<int, 2>;

  int sn = 0;
  hsv_t hue;
  hsv_t sat;
  hsv_t val;
  double exposure;

  /**
   * Default constructor.
   */
  PipelineConfig();

  /**
   * Constructor from member values.
   */
  PipelineConfig(int sn, hsv_t hue, hsv_t sat, hsv_t val, double exposure);

  /**
   * Constructor from NetworkTables.
   */
  PipelineConfig(std::shared_ptr<nt::Value> value);

  /**
   * New is factory method to create a PipelineConfig from a NT value.
   */
  static PipelineConfig New(std::shared_ptr<nt::Value> value);

  template <typename OStream>
  friend OStream& operator<<(OStream& os, PipelineConfig const& pc) {
    os << "PipelineConfig{sn=" << pc.sn << ", hue=[" << pc.hue[0] << ","
       << pc.hue[1] << "], "
       << "sat=[" << pc.sat[0] << "," << pc.sat[1] << "], "
       << "val=[" << pc.val[0] << "," << pc.val[1]
       << "], exposure=" << pc.exposure << "}";
    return os;
  }
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
