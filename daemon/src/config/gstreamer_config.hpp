#pragma once
#include <spdlog/fmt/ostr.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

namespace deadeye {

struct GStreamerConfig {
  static char const* kCaptureWidthKey;
  static char const* kCaptureHeightKey;
  static char const* kOutputWidthKey;
  static char const* kOutputHeightKey;
  static char const* kFrameRateKey;
  static char const* kFlipModeKey;

  int capture_width = 0;
  int capture_height = 0;
  int output_width = 0;
  int output_height = 0;
  int frame_rate = 0;
  int flip_mode = 0;
  std::array<int, 2> exposure{13000, 683709000};

  /**
   * Default constructor.
   */
  GStreamerConfig();

  /**
   * Constructor from member values.
   */
  GStreamerConfig(int capture_width, int capture_height, int output_width,
                  int output_height, int frame_rate, int flip_mode);

  /*
   * Get Jetson CSI Camera pipeline.
   */
  std::string GetJetsonCSI();

  template <typename OStream>
  friend OStream& operator<<(OStream& os, GStreamerConfig const& gsc) {
    os << "GStreamerConfig{"
       << "cw=" << gsc.capture_width << ", ch=" << gsc.capture_height
       << +", ow=" << gsc.output_width << ", oh=" << gsc.output_height
       << ", fps=" << gsc.frame_rate << ", flip=" << gsc.flip_mode << "}";
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
         lhs.frame_rate == rhs.frame_rate && lhs.flip_mode == rhs.flip_mode;
}

inline bool operator!=(GStreamerConfig const& lhs, GStreamerConfig const& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
