#include "capture/usb_cscore.h"

#include <cscore_oo.h>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

using namespace deadeye;
using json = nlohmann::json;

UsbCsCore::UsbCsCore(const CaptureConfig& config) {
  json j = config.config;
  if (!j.is_object()) j = json::object();

  auto device = j.value("device", "/dev/video0");
  cs::UsbCamera camera{"usbcamera", device};
  camera.SetVideoMode(cs::VideoMode::kYUYV, config.width, config.height,
                      config.frame_rate);

  auto info = camera.GetInfo();
  spdlog::info("UsbCsCore: camera={}, device={}", info.name, info.path);

  //   auto json = camera.GetConfigJson();
  //   spdlog::debug("UsbCsCore: {}", json);

  cv_sink_.SetSource(camera);
}

bool UsbCsCore::Grab(cv::Mat& frame) {
  auto time = cv_sink_.GrabFrame(frame, 1.0);

  if (time == 0) {
    spdlog::error("UsbCsCore: error: {}", cv_sink_.GetError());
    return false;
  }

  return true;
}