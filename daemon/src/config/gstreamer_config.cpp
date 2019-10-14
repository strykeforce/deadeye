#include "gstreamer_config.hpp"

using namespace deadeye;
using json = nlohmann::json;

char const* GStreamerConfig::kCaptureWidthKey{"cw"};
char const* GStreamerConfig::kCaptureHeightKey{"ch"};
char const* GStreamerConfig::kOutputWidthKey{"ow"};
char const* GStreamerConfig::kOutputHeightKey{"oh"};
char const* GStreamerConfig::kFrameRateKey{"fps"};
char const* GStreamerConfig::kFlipModeKey{"flip"};

GStreamerConfig::GStreamerConfig() {}

GStreamerConfig::GStreamerConfig(int capture_width, int capture_height,
                                 int output_width, int output_height,
                                 int frame_rate, int flip_mode)
    : capture_width(capture_width),
      capture_height(capture_height),
      output_width(output_width),
      output_height(output_height),
      frame_rate(frame_rate),
      flip_mode(flip_mode) {}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const GStreamerConfig& g) {
  j = json{{GStreamerConfig::kCaptureWidthKey, g.capture_width},
           {GStreamerConfig::kCaptureHeightKey, g.capture_height},
           {GStreamerConfig::kOutputWidthKey, g.output_width},
           {GStreamerConfig::kOutputHeightKey, g.output_height},
           {GStreamerConfig::kFrameRateKey, g.frame_rate},
           {GStreamerConfig::kFlipModeKey, g.flip_mode}};
}

void deadeye::from_json(const json& j, GStreamerConfig& g) {
  j.at(GStreamerConfig::kCaptureWidthKey).get_to(g.capture_width);
  j.at(GStreamerConfig::kCaptureHeightKey).get_to(g.capture_height);
  j.at(GStreamerConfig::kOutputWidthKey).get_to(g.output_width);
  j.at(GStreamerConfig::kOutputHeightKey).get_to(g.output_height);
  j.at(GStreamerConfig::kFrameRateKey).get_to(g.frame_rate);
  j.at(GStreamerConfig::kFlipModeKey).get_to(g.flip_mode);
}
