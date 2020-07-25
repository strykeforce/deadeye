#pragma once

#include <nlohmann/json.hpp>

namespace deadeye {
enum class CaptureType { autosrc, jetson, osx, test };

NLOHMANN_JSON_SERIALIZE_ENUM(CaptureType, {
                                              {CaptureType::jetson, "jetson"},
                                              {CaptureType::osx, "osx"},
                                              {CaptureType::test, "test"},
                                          });
}  // namespace deadeye
