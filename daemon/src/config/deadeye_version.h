#pragma once
#include <string>

namespace deadeye {

std::string GetDeadeyeVersion();

unsigned GetDeadeyeVersionMajor();

unsigned GetDeadeyeVersionMinor();

unsigned GetDeadeyeVersionPatch();

unsigned GetDeadeyeVersionTweak();

unsigned GetDeadeyeCommitsSinceVersionChange();

}  // namespace deadeye
