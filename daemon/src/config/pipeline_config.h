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

namespace {
std::string onoff(bool state) { return state ? "(on)" : "(off)"; }
}  // namespace

namespace deadeye {

struct PipelineConfig {
  static constexpr auto kSerialKey = "sn";
  static constexpr auto kHsvHueKey = "hue";
  static constexpr auto kHsvSatKey = "sat";
  static constexpr auto kHsvValKey = "val";
  static constexpr auto kFilterKey = "filter";
  static constexpr auto kLogKey = "log";

  using hsv_t = std::array<int, 2>;

  int sn = 0;
  hsv_t hue{-1, -1};
  hsv_t sat{-1, -1};
  hsv_t val{-1, -1};
  FilterConfig filter;
  LogConfig log;

  /**
   * Default constructor.
   */
  PipelineConfig() = default;

  /**
   * Constructor from member values.
   */
  PipelineConfig(int sn, hsv_t hue, hsv_t sat, hsv_t val, FilterConfig filter,
                 LogConfig log);

  /**
   * Constructor from NetworkTables.
   */
  explicit PipelineConfig(const std::shared_ptr<nt::Value>& value);

  [[nodiscard]] cv::Scalar HsvLow() const {
    return {static_cast<double>(hue[0]), static_cast<double>(sat[0]),
            static_cast<double>(val[0])};
  }
  [[nodiscard]] cv::Scalar HsvHigh() const {
    return {static_cast<double>(hue[1]), static_cast<double>(sat[1]),
            static_cast<double>(val[1])};
  }

  template <typename OStream>
  friend OStream& operator<<(OStream& os, const PipelineConfig& pc) {
    std::string output = fmt::format(
        "PipelineConfig<sn={}, hue=[{},{}], sat=[{},{}], val=[{},{}], "
        "filter=<area=[{},{}] {}, solidity=[{},{}] {}, aspect=[{},{}] "
        "{}>, log=<{}, "
        "{}>>",
        pc.sn, pc.hue[0], pc.hue[1], pc.sat[0], pc.sat[1], pc.val[0], pc.val[0],
        pc.filter.area[0], pc.filter.area[1], onoff(pc.filter.IsAreaEnabled()),
        pc.filter.solidity[0], pc.filter.solidity[1],
        onoff(pc.filter.IsSolidityEnabled()), pc.filter.aspect[0],
        pc.filter.aspect[1], onoff(pc.filter.IsAspectEnabled()), pc.log.path,
        pc.log.fps);
    os << output;
    return os;
  }
};

void to_json(nlohmann::json& j, const PipelineConfig& p);
void from_json(const nlohmann::json& j, PipelineConfig& p);

inline bool operator==(const PipelineConfig& lhs, const PipelineConfig& rhs) {
  return lhs.sn == rhs.sn && lhs.hue == rhs.hue && lhs.sat == rhs.sat &&
         lhs.val == rhs.val && lhs.filter == rhs.filter && lhs.log == rhs.log;
}

inline bool operator!=(const PipelineConfig& lhs, const PipelineConfig& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
