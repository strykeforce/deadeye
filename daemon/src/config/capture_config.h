#pragma once
#include <fmt/core.h>
#include <networktables/NetworkTableValue.h>
#include <spdlog/fmt/ostr.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace deadeye {

struct CaptureConfig {
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
  CaptureConfig() = default;

  /**
   * Constructor from member values.
   */
  CaptureConfig(Type type, int capture_width, int capture_height,
                int output_width, int output_height, int frame_rate,
                int flip_mode, double exposure);

  /**
   * Constructor from NetworkTables.
   */
  CaptureConfig(std::shared_ptr<nt::Value> value);

  /*
   * Get Jetson CSI Camera pipeline.
   */
  std::string Pipeline() const;

  std::string PipelineType() const;

  template <typename OStream>
  friend OStream& operator<<(OStream& os, CaptureConfig const& cc) {
    std::string output = fmt::format(
        "CaptureConfig<type={}, cw={}, ch={}, ow={}, oh={}, fps={}, flip={}, "
        "exp={}>",
        cc.PipelineType(), cc.capture_width, cc.capture_height, cc.output_width,
        cc.output_height, cc.frame_rate, cc.flip_mode, cc.exposure);
    os << output;
    return os;
  }
};

void to_json(json& j, const CaptureConfig& cc);
void from_json(const json& j, CaptureConfig& cc);

inline bool operator==(CaptureConfig const& lhs, CaptureConfig const& rhs) {
  return lhs.capture_width == rhs.capture_width &&
         lhs.capture_height == rhs.capture_height &&
         lhs.output_width == rhs.output_width &&
         lhs.output_height == rhs.output_height &&
         lhs.frame_rate == rhs.frame_rate && lhs.flip_mode == rhs.flip_mode &&
         lhs.exposure == rhs.exposure;
}

inline bool operator!=(CaptureConfig const& lhs, CaptureConfig const& rhs) {
  return !(lhs == rhs);
}

NLOHMANN_JSON_SERIALIZE_ENUM(CaptureConfig::Type,
                             {
                                 {CaptureConfig::Type::jetson, "jetson"},
                                 {CaptureConfig::Type::osx, "osx"},
                                 {CaptureConfig::Type::test, "test"},
                             })

}  // namespace deadeye
