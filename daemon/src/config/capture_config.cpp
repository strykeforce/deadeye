#include "config/capture_config.h"

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

char const* CaptureConfig::kTypeKey{"type"};
char const* CaptureConfig::kCaptureWidthKey{"cw"};
char const* CaptureConfig::kCaptureHeightKey{"ch"};
char const* CaptureConfig::kOutputWidthKey{"ow"};
char const* CaptureConfig::kOutputHeightKey{"oh"};
char const* CaptureConfig::kFrameRateKey{"fps"};
char const* CaptureConfig::kFlipModeKey{"flip"};
char const* CaptureConfig::kExposureKey{"exp"};

CaptureConfig::CaptureConfig(Type type, int capture_width, int capture_height,
                             int output_width, int output_height,
                             int frame_rate, int flip_mode, double exposure)
    : type(type),
      capture_width(capture_width),
      capture_height(capture_height),
      output_width(output_width),
      output_height(output_height),
      frame_rate(frame_rate),
      flip_mode(flip_mode),
      exposure(exposure) {}

std::string CaptureConfig::Pipeline() const {
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

std::string CaptureConfig::PipelineType() const {
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
void deadeye::to_json(json& j, const CaptureConfig& cc) {
  j = json{{CaptureConfig::kTypeKey, cc.type},
           {CaptureConfig::kCaptureWidthKey, cc.capture_width},
           {CaptureConfig::kCaptureHeightKey, cc.capture_height},
           {CaptureConfig::kOutputWidthKey, cc.output_width},
           {CaptureConfig::kOutputHeightKey, cc.output_height},
           {CaptureConfig::kFrameRateKey, cc.frame_rate},
           {CaptureConfig::kFlipModeKey, cc.flip_mode},
           {CaptureConfig::kExposureKey, cc.exposure}};
}

void deadeye::from_json(const json& j, CaptureConfig& cc) {
  j.at(CaptureConfig::kTypeKey).get_to(cc.type);
  j.at(CaptureConfig::kCaptureWidthKey).get_to(cc.capture_width);
  j.at(CaptureConfig::kCaptureHeightKey).get_to(cc.capture_height);
  j.at(CaptureConfig::kOutputWidthKey).get_to(cc.output_width);
  j.at(CaptureConfig::kOutputHeightKey).get_to(cc.output_height);
  j.at(CaptureConfig::kFrameRateKey).get_to(cc.frame_rate);
  j.at(CaptureConfig::kFlipModeKey).get_to(cc.flip_mode);
  j.at(CaptureConfig::kExposureKey).get_to(cc.exposure);
}
