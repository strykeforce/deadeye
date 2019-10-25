#pragma once
#include <fmt/core.h>
#include <spdlog/fmt/ostr.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

namespace deadeye {

struct GStreamerConfig {
  enum class Type { jetson, osx, test };

  static char const* kTypeKey;
  static char const* kCaptureWidthKey;
  static char const* kCaptureHeightKey;
  static char const* kOutputWidthKey;
  static char const* kOutputHeightKey;
  static char const* kFrameRateKey;
  static char const* kFlipModeKey;
  static char const* kExposureKey;

  Type type = Type::test;
  int capture_width = 0;
  int capture_height = 0;
  int output_width = 0;
  int output_height = 0;
  int frame_rate = 0;
  int flip_mode = 0;
  double exposure = 0.0;

  /**
   * Default constructor.
   */
  GStreamerConfig() = default;

  /**
   * Constructor from member values.
   */
  GStreamerConfig(Type type, int capture_width, int capture_height,
                  int output_width, int output_height, int frame_rate,
                  int flip_mode, double exposure);

  /*
   * Get Jetson CSI Camera pipeline.
   */
  std::string Pipeline() const;

  std::string PipelineType() const;

  template <typename OStream>
  friend OStream& operator<<(OStream& os, GStreamerConfig const& gsc) {
    std::string output = fmt::format(
        "GStreamerConfig<type={}, cw={}, ch={}, ow={}, oh={}, fps={}, flip={}, "
        "exp={}>",
        gsc.PipelineType(), gsc.capture_width, gsc.capture_height,
        gsc.output_width, gsc.output_height, gsc.frame_rate, gsc.flip_mode,
        gsc.exposure);
    os << output;
    return os;
  }
};

void to_json(json& j, const GStreamerConfig& l);
void from_json(const json& j, GStreamerConfig& l);

inline bool operator==(GStreamerConfig const& lhs, GStreamerConfig const& rhs) {
  return lhs.capture_width == rhs.capture_width &&
         lhs.capture_height == rhs.capture_height &&
         lhs.output_width == rhs.output_width &&
         lhs.output_height == rhs.output_height &&
         lhs.frame_rate == rhs.frame_rate && lhs.flip_mode == rhs.flip_mode &&
         lhs.exposure == rhs.exposure;
}

inline bool operator!=(GStreamerConfig const& lhs, GStreamerConfig const& rhs) {
  return !(lhs == rhs);
}

NLOHMANN_JSON_SERIALIZE_ENUM(GStreamerConfig::Type,
                             {
                                 {GStreamerConfig::Type::jetson, "jetson"},
                                 {GStreamerConfig::Type::osx, "osx"},
                                 {GStreamerConfig::Type::test, "test"},
                             })

}  // namespace deadeye
