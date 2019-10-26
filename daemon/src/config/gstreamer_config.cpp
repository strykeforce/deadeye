#include "gstreamer_config.h"

#include <fmt/core.h>

using namespace deadeye;
using json = nlohmann::json;

namespace {
static const bool kExposureLock = true;
static const bool kWhiteBalanceLock = false;
static const int kWhiteBalanceMode = 0;
static const std::string kIspDigitalGainRange = "1 1";
static const std::string kGainRange = "1 1";
static const std::array<int, 2> kExposureRange{13000, 8333333};
}  // namespace

char const* GStreamerConfig::kTypeKey{"type"};
char const* GStreamerConfig::kCaptureWidthKey{"cw"};
char const* GStreamerConfig::kCaptureHeightKey{"ch"};
char const* GStreamerConfig::kOutputWidthKey{"ow"};
char const* GStreamerConfig::kOutputHeightKey{"oh"};
char const* GStreamerConfig::kFrameRateKey{"fps"};
char const* GStreamerConfig::kFlipModeKey{"flip"};
char const* GStreamerConfig::kExposureKey{"exp"};

GStreamerConfig::GStreamerConfig(Type type, int capture_width,
                                 int capture_height, int output_width,
                                 int output_height, int frame_rate,
                                 int flip_mode, double exposure)
    : capture_width(capture_width),
      capture_height(capture_height),
      output_width(output_width),
      output_height(output_height),
      frame_rate(frame_rate),
      flip_mode(flip_mode),
      exposure(exposure) {}

std::string GStreamerConfig::Pipeline() const {
  switch (type) {
    case Type::jetson: {
      std::string jetson{
          R"(nvarguscamerasrc aelock={} awblock={} wbmode={})"
          R"( ispdigitalgainrange="{}" gainrange="{}" exposuretimerange="{} {}" !)"
          R"( video/x-raw(memory:NVMM), width=(int){}, height=(int){},)"
          R"( format=(string)NV12, framerate=(fraction){}/1 ! nvvidconv)"
          R"( flip-method={} ! video/x-raw, width=(int){}, height=(int){},)"
          R"( format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink)"};

      int exp =
          static_cast<int>(exposure * (kExposureRange[1] - kExposureRange[0]));

      return fmt::format(jetson, kExposureLock, kWhiteBalanceLock,
                         kWhiteBalanceMode, kIspDigitalGainRange, kGainRange,
                         exp, exp, capture_width, capture_height, frame_rate,
                         flip_mode, output_width, output_height);
    }

    case Type::osx:
      return "autovideosrc ! videoconvert ! appsink";

    case Type::test:
      std::string test{
          "videotestsrc ! video/x-raw, width={}, "
          "height={}, "
          "framerate={}/1 ! videoconvert ! video/x-raw, format=(string)BGR ! "
          "appsink"};
      return fmt::format(test, output_width, output_height, frame_rate);
  };
  return "GSTREAMER TYPE NOT FOUND";
}

std::string GStreamerConfig::PipelineType() const {
  switch (type) {
    case Type::jetson:
      return "jetson";
    case Type::osx:
      return "osx";
    case Type::test:
      return "test";
  }
  return "";
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const GStreamerConfig& g) {
  j = json{{GStreamerConfig::kTypeKey, g.type},
           {GStreamerConfig::kCaptureWidthKey, g.capture_width},
           {GStreamerConfig::kCaptureHeightKey, g.capture_height},
           {GStreamerConfig::kOutputWidthKey, g.output_width},
           {GStreamerConfig::kOutputHeightKey, g.output_height},
           {GStreamerConfig::kFrameRateKey, g.frame_rate},
           {GStreamerConfig::kFlipModeKey, g.flip_mode},
           {GStreamerConfig::kExposureKey, g.exposure}};
}

void deadeye::from_json(const json& j, GStreamerConfig& g) {
  j.at(GStreamerConfig::kTypeKey).get_to(g.type);
  j.at(GStreamerConfig::kCaptureWidthKey).get_to(g.capture_width);
  j.at(GStreamerConfig::kCaptureHeightKey).get_to(g.capture_height);
  j.at(GStreamerConfig::kOutputWidthKey).get_to(g.output_width);
  j.at(GStreamerConfig::kOutputHeightKey).get_to(g.output_height);
  j.at(GStreamerConfig::kFrameRateKey).get_to(g.frame_rate);
  j.at(GStreamerConfig::kFlipModeKey).get_to(g.flip_mode);
  j.at(GStreamerConfig::kExposureKey).get_to(g.exposure);
}
