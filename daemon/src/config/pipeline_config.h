#pragma once
#include <fmt/core.h>
#include <networktables/NetworkTableValue.h>
#include <spdlog/fmt/ostr.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>
#include <string>

#include "config/filter_config.h"
#include "config/log_config.h"

using json = nlohmann::json;

namespace deadeye {

struct PipelineConfig {
  static char const* kSerialKey;
  static char const* kHsvHueKey;
  static char const* kHsvSatKey;
  static char const* kHsvValKey;
  static char const* kExposureKey;
  static char const* kFilterKey;
  static char const* kLogKey;

  using hsv_t = std::array<int, 2>;

  int sn = 0;
  hsv_t hue{-1, -1};
  hsv_t sat{-1, -1};
  hsv_t val{-1, -1};
  double exposure{-1.0};  // FIXME: unused, remove
  FilterConfig filter;
  LogConfig log;

  /**
   * Default constructor.
   */
  PipelineConfig() = default;

  /**
   * Constructor from member values.
   */
  PipelineConfig(int sn, hsv_t hue, hsv_t sat, hsv_t val, double exposure,
                 FilterConfig filter, LogConfig log);

  /**
   * Constructor from NetworkTables.
   */
  PipelineConfig(std::shared_ptr<nt::Value> value);

  cv::Scalar HsvLow() const { return cv::Scalar(hue[0], sat[0], val[0]); }
  cv::Scalar HsvHigh() const { return cv::Scalar(hue[1], sat[1], val[1]); }

  template <typename OStream>
  friend OStream& operator<<(OStream& os, PipelineConfig const& pc) {
    std::string output = fmt::format(
        "PipelineConfig<sn={}, hue=[{},{}], sat=[{},{}], val=[{},{}], "
        "filter=<area=[{},{}], fullness=[{},{}], aspect=[{},{}]>, log=<{}, "
        "{}>>",
        pc.sn, pc.hue[0], pc.hue[1], pc.sat[0], pc.sat[1], pc.val[0], pc.val[0],
        pc.filter.area[0], pc.filter.area[1], pc.filter.fullness[0],
        pc.filter.fullness[1], pc.filter.aspect[0], pc.filter.aspect[1],
        pc.log.path, pc.log.enabled);
    os << output;
    return os;
  }
};

void to_json(json& j, const PipelineConfig& p);
void from_json(const json& j, PipelineConfig& p);

inline bool operator==(PipelineConfig const& lhs, PipelineConfig const& rhs) {
  return lhs.sn == rhs.sn && lhs.hue == rhs.hue && lhs.sat == rhs.sat &&
         lhs.val == rhs.val && lhs.exposure == rhs.exposure &&
         lhs.filter == rhs.filter && lhs.log == rhs.log;
}

inline bool operator!=(PipelineConfig const& lhs, PipelineConfig const& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
