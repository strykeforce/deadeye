#include "log_config.h"

using namespace deadeye;
using json = nlohmann::json;

char const* LogConfig::kPathKey{"path"};
char const* LogConfig::kMountKey{"mount"};
char const* LogConfig::kEnabledKey{"enabled"};
char const* LogConfig::kDefaultPath{"/mnt/deadeye"};

LogConfig::LogConfig(std::string path, bool enabled, bool mount)
    : path(path), enabled(enabled), mount(mount) {}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const LogConfig& l) {
  j = json{{LogConfig::kPathKey, l.path},
           {LogConfig::kEnabledKey, l.enabled},
           {LogConfig::kMountKey, l.mount}};
}

void deadeye::from_json(const json& j, LogConfig& l) {
  if (j.contains(LogConfig::kPathKey))
    j.at(LogConfig::kPathKey).get_to(l.path);
  else
    l.path = LogConfig::kDefaultPath;

  if (j.contains(LogConfig::kMountKey))
    j.at(LogConfig::kMountKey).get_to(l.mount);
  else
    l.mount = true;

  j.at(LogConfig::kEnabledKey).get_to(l.enabled);
}
