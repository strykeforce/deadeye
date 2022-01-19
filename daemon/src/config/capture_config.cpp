#include "config/capture_config.h"

#include <fmt/core.h>

#include <nlohmann/json.hpp>
#include <utility>

using namespace deadeye;
using json = nlohmann::json;

CaptureConfig::CaptureConfig(CaptureType type, int width, int height,
                             int frame_rate, json config)
    : type(type),
      width(width),
      height(height),
      frame_rate(frame_rate),
      config(std::move(config)) {}

CaptureConfig::CaptureConfig(const std::shared_ptr<nt::Value>& value) {
  assert(value);
  auto j = json::parse(value->GetString());
  j.at(kTypeKey).get_to(type);
  j.at(kWidthKey).get_to(width);
  j.at(kHeightKey).get_to(height);
  j.at(kFrameRateKey).get_to(frame_rate);
  config = j.at(CaptureConfig::kConfigKey).get<json>();
}

std::string CaptureConfig::PipelineType() const {
  switch (type) {
    case CaptureType::jetson:
      return "jetson";
    case CaptureType::test:
      return "test";
    case CaptureType::file:
      return "file";
    case CaptureType::usb:
      return "usb";
    default:
      return "UNKNOWN";
  }
}

cv::Size CaptureConfig::Size() const { return cv::Size{width, height}; }

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const CaptureConfig& cc) {
  j = json{{CaptureConfig::kTypeKey, cc.type},
           {CaptureConfig::kWidthKey, cc.width},
           {CaptureConfig::kHeightKey, cc.height},
           {CaptureConfig::kFrameRateKey, cc.frame_rate},
           {CaptureConfig::kConfigKey, cc.config}};
}

void deadeye::from_json(const json& j, CaptureConfig& cc) {
  j.at(CaptureConfig::kTypeKey).get_to(cc.type);
  j.at(CaptureConfig::kWidthKey).get_to(cc.width);
  j.at(CaptureConfig::kHeightKey).get_to(cc.height);
  j.at(CaptureConfig::kFrameRateKey).get_to(cc.frame_rate);
  cc.config = j.at(CaptureConfig::kConfigKey).get<json>();
}
