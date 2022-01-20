#pragma once

#include <nlohmann/json.hpp>

namespace deadeye {
enum class CaptureType { jetson, test, file, usb };

NLOHMANN_JSON_SERIALIZE_ENUM(CaptureType, {
                                              {CaptureType::jetson, "jetson"},
                                              {CaptureType::test, "test"},
                                              {CaptureType::file, "file"},
                                              {CaptureType::usb, "usb"},
                                          })
}  // namespace deadeye
