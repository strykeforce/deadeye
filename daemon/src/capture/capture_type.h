#pragma once

#include <nlohmann/json.hpp>

namespace deadeye {
enum class CaptureType { jetson, test, file };

NLOHMANN_JSON_SERIALIZE_ENUM(CaptureType, {
                                              {CaptureType::jetson, "jetson"},
                                              {CaptureType::test, "test"},
                                              {CaptureType::file, "file"},
                                          });
}  // namespace deadeye
