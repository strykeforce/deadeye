#pragma once

#include <nlohmann/json.hpp>

namespace deadeye {

enum class LogType { capture, four_up };

NLOHMANN_JSON_SERIALIZE_ENUM(LogType, {{LogType::capture, "capture"},
                                       {LogType::four_up, "four_up"}})

}  // namespace deadeye
