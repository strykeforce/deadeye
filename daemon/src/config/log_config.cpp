#include "log_config.h"

using namespace deadeye;
using json = nlohmann::json;

char const* LogConfig::kPathKey{"path"};
char const* LogConfig::kEnabledKey{"enabled"};
char const* LogConfig::kDefaultPath{"/mnt/deadeye"};

LogConfig::LogConfig(std::string path, bool enabled)
    : path(path), enabled(enabled) {}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const LogConfig& l) {
  j = json{{LogConfig::kPathKey, l.path}, {LogConfig::kEnabledKey, l.enabled}};
}

void deadeye::from_json(const json& j, LogConfig& l) {
  if (j.contains(LogConfig::kPathKey))
    j.at(LogConfig::kPathKey).get_to(l.path);
  else
    l.path = LogConfig::kDefaultPath;
  j.at(LogConfig::kEnabledKey).get_to(l.enabled);
}
