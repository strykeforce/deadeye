#include "capture/usb_cscore.h"

#include <cscore_oo.h>
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

  auto brightness = j.value("brightness", camera.GetBrightness());  // 0 - 100
  camera.SetBrightness(brightness);

  auto ae_lock = j.value("aeLock", true);  // auto exposure
  if (ae_lock) {
    camera.SetExposureAuto();
  } else {
    auto exposure = j.value("exposure", 25);  // 0 - 100
    camera.SetExposureManual(exposure);
  }

  auto awb_lock = j.value("awbLock", true);  // auto white balance
  if (awb_lock) {
    camera.SetWhiteBalanceAuto();
  } else {
    auto wb = j.value("wbTemp", 5'000);  // 2,000 - 10,000
    camera.SetWhiteBalanceManual(wb);
  }

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