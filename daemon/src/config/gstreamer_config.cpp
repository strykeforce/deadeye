#include "gstreamer_config.hpp"

#include <fmt/core.h>

using namespace deadeye;
using json = nlohmann::json;

namespace {
static const int kWhiteBalanceMode = 0;
static const std::string kIspDigitalGainRange = "1 1";
static const std::string kGainRange = "1 2";
}  // namespace

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

std::string GStreamerConfig::GetJetsonCSI() {
  std::string tmplt =
      R"(nvarguscamerasrc awblock=true aelock=true wbmode={})"
      R"( ispdigitalgainrange="{}" gainrange="{}" exposuretimerange="{} {}" !)"
      R"( video/x-raw(memory:NVMM), width=(int){}, height=(int){},)"
      R"( format=(string)NV12, framerate=(fraction){}/1 ! nvvidconv)"
      R"( flip-method={} ! video/x-raw, width=(int){}, height=(int){},)"
      R"( format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink)";
  return fmt::format(tmplt, kWhiteBalanceMode, kIspDigitalGainRange, kGainRange,
                     exposure[0], exposure[1], capture_width, capture_height,
                     frame_rate, flip_mode, output_width, output_height);
}

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
