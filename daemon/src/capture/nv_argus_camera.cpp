#include "capture/nv_argus_camera.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

using namespace deadeye;
using json = nlohmann::json;

NvArgusCamera::NvArgusCamera(const CaptureConfig& config) {
  auto pipeline = NvArgusCameraPipeline(config);
  is_yuv_ = true;
  cap_.open(pipeline, cv::CAP_GSTREAMER);
  spdlog::debug("NvArgusCamera: opened cap_: {}", pipeline);
}

std::string deadeye::NvArgusCameraPipeline(const CaptureConfig& config) {
  json j = config.config;
  if (!j.is_object()) j = json::object();

  auto silent = j.value("silent", true);
  auto wb_mode = j.value("wbMode", 0);
  auto saturation = j.value("saturation", 1.0);

  auto exposure = static_cast<int>(
      j.value("exposure", 1.0) * (8'333'333 - 13'000) + 13'000);
  auto exposure_range = fmt::format("{} {}", exposure, exposure);

  auto gain_range = j.value("gainRange", "1 1");
  auto digital_gain_range = j.value("ispDigitalGainRange", "1 1");
  auto ae_lock = j.value("aeLock", true);
  auto awb_lock = j.value("awbLock", false);

  auto src_fmt =
      "nvarguscamerasrc "
      "silent={} "
      "wbmode={} "
      "saturation={} "
      "exposuretimerange=\"{}\" "
      "gainrange=\"{}\" "
      "ispdigitalgainrange=\"{}\" "
      "aelock={} "
      "awblock={}";

  auto src = fmt::format(src_fmt, silent, wb_mode, saturation, exposure_range,
                         gain_range, digital_gain_range, ae_lock, awb_lock);

  auto cap_width = j.value("captureWidth", 1280);
  auto cap_height = j.value("captureHeight", 720);

  auto cap_format_fmt =
      "video/x-raw(memory:NVMM),"
      "width=(int){},"
      "height=(int){},"
      "framerate=(fraction){}/1,"
      "format=(string)NV12";

  auto cap_format =
      fmt::format(cap_format_fmt, cap_width, cap_height, config.frame_rate);

  auto flip_method = j.value("flipMethod", 0);

  auto output_fmt =
      "nvvidconv "
      "flip-method={} ! "
      "video/x-raw,format=UYVY,"
      "width=(int){},"
      "height=(int){}";

  auto output =
      fmt::format(output_fmt, flip_method, config.width, config.height);

  return src + " ! " + cap_format + " ! " + output + " ! appsink";
}
