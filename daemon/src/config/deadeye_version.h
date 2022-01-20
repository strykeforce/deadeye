#pragma once
#include <string>

namespace deadeye {

std::string GetDeadeyeVersion();
[[maybe_unused]] unsigned GetDeadeyeVersionMajor();
[[maybe_unused]] unsigned GetDeadeyeVersionMinor();
[[maybe_unused]] unsigned GetDeadeyeVersionPatch();
[[maybe_unused]] unsigned GetDeadeyeVersionTweak();

}  // namespace deadeye
