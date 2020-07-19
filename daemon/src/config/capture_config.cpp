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
// exposure time max is 1/120 fps in nanosec
static const std::array<int, 2> kExposureRange{13000, 8333333};
}  // namespace

const char* CaptureConfig::kTypeKey{"type"};
const char* CaptureConfig::kCaptureWidthKey{"cw"};
const char* CaptureConfig::kCaptureHeightKey{"ch"};
const char* CaptureConfig::kOutputWidthKey{"ow"};
const char* CaptureConfig::kOutputHeightKey{"oh"};
const char* CaptureConfig::kFrameRateKey{"fps"};
const char* CaptureConfig::kFlipModeKey{"flip"};
const char* CaptureConfig::kExposureKey{"exp"};

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

CaptureConfig::CaptureConfig(std::shared_ptr<nt::Value> value) {
  assert(value);
  auto j = json::parse(value->GetString().str());
  j.at(kTypeKey).get_to(type);
  j.at(kCaptureWidthKey).get_to(capture_width);
  j.at(kCaptureHeightKey).get_to(capture_height);
  j.at(kOutputWidthKey).get_to(output_width);
  j.at(kOutputHeightKey).get_to(output_height);
  j.at(kFrameRateKey).get_to(frame_rate);
  j.at(kFlipModeKey).get_to(flip_mode);
  j.at(kExposureKey).get_to(exposure);
}

std::string CaptureConfig::Pipeline() const {
  switch (type) {
    case Type::autosrc:
      return "autovideosrc ! videoconvert ! appsink";

    case Type::jetson: {
      // `gst-inspect-1.0 nvarguscamerasrc` to list supported properties
      std::string jetson{
          R"(nvarguscamerasrc aelock={} awblock={} wbmode={})"
          R"( ispdigitalgainrange="{}" gainrange="{}" exposuretimerange="{} {}" !)"
          R"( video/x-raw(memory:NVMM), width=(int){}, height=(int){},)"
          R"( format=(string)NV12, framerate=(fraction){}/1 ! nvvidconv)"
          R"( flip-method={} ! video/x-raw, format=I420, width=(int){}, height=(int){})"
          R"( ! appsink)"};

      int exp =
          static_cast<int>(exposure * (kExposureRange[1] - kExposureRange[0]));

      return fmt::format(jetson, kExposureLock, kWhiteBalanceLock,
                         kWhiteBalanceMode, kIspDigitalGainRange, kGainRange,
                         exp, exp, capture_width, capture_height, frame_rate,
                         flip_mode, output_width, output_height);
    }

    case Type::osx: {
      std::string osx{
          "avfvideosrc ! video/x-raw,width={},height={} ! videoconvert ! "
          "video/x-raw, format=(string)BGR ! appsink"};
      return fmt::format(osx, output_width, output_height);
    }

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
    case Type::autosrc:
      return "auto";
    case Type::jetson:
      return "jetson";
    case Type::osx:
      return "osx";
    case Type::test:
      return "test";
  }
  return "";
}

cv::Size CaptureConfig::CaptureSize() const {
  return cv::Size{capture_width, capture_height};
}

cv::Size CaptureConfig::OutputSize() const {
  return cv::Size{output_width, output_height};
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
