// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <nlohmann/json.hpp>

namespace deadeye {

enum class FrameLogType { capture, four_up };

NLOHMANN_JSON_SERIALIZE_ENUM(FrameLogType, {{FrameLogType::capture, "capture"},
                                            {FrameLogType::four_up, "four_up"}})

}  // namespace deadeye
