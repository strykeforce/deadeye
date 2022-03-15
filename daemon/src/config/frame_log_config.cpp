#include "frame_log_config.h"

#include <utility>

using namespace deadeye;
using json = nlohmann::json;

FrameLogConfig::FrameLogConfig(FrameLogType type, std::string path, int fps,
                               bool mount)
    : type(type), path(std::move(path)), fps(fps), mount(mount) {}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const FrameLogConfig& l) {
  j = json{{FrameLogConfig::kTypeKey, l.type},
           {FrameLogConfig::kPathKey, l.path},
           {FrameLogConfig::kFpsKey, l.fps},
           {FrameLogConfig::kMountKey, l.mount}};
}

void deadeye::from_json(const json& j, FrameLogConfig& l) {
  if (j.contains(FrameLogConfig::kTypeKey))
    j.at(FrameLogConfig::kTypeKey).get_to(l.type);
  else
    l.type = FrameLogType::four_up;

  if (j.contains(FrameLogConfig::kPathKey))
    j.at(FrameLogConfig::kPathKey).get_to(l.path);
  else
    l.path = FrameLogConfig::kDefaultPath;

  if (j.contains(FrameLogConfig::kMountKey))
    j.at(FrameLogConfig::kMountKey).get_to(l.mount);
  else
    l.mount = true;

  j.at(FrameLogConfig::kFpsKey).get_to(l.fps);
}
