#include "log_config.h"

using namespace deadeye;
using json = nlohmann::json;

LogConfig::LogConfig(LogType type, std::string path, int fps, bool mount)
    : type(type), path(path), fps(fps), mount(mount) {}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const LogConfig& l) {
  j = json{{LogConfig::kTypeKey, l.type},
           {LogConfig::kPathKey, l.path},
           {LogConfig::kFpsKey, l.fps},
           {LogConfig::kMountKey, l.mount}};
}

void deadeye::from_json(const json& j, LogConfig& l) {
  if (j.contains(LogConfig::kTypeKey))
    j.at(LogConfig::kTypeKey).get_to(l.type);
  else
    l.type = LogType::four_up;

  if (j.contains(LogConfig::kPathKey))
    j.at(LogConfig::kPathKey).get_to(l.path);
  else
    l.path = LogConfig::kDefaultPath;

  if (j.contains(LogConfig::kMountKey))
    j.at(LogConfig::kMountKey).get_to(l.mount);
  else
    l.mount = true;

  j.at(LogConfig::kFpsKey).get_to(l.fps);
}
