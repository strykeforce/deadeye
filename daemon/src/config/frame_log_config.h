// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "log/frame_log_type.h"

namespace deadeye {

struct FrameLogConfig {
  static constexpr auto kTypeKey = "type";
  static constexpr auto kFpsKey = "fps";
  static constexpr auto kPathKey = "path";
  static constexpr auto kMountKey = "mount";
  static constexpr auto kDefaultPath = "/var/opt/deadeye";

  FrameLogType type{};
  std::string path{kDefaultPath};
  int fps{0};
  bool mount{true};

  /**
   * Default constructor.
   */
  FrameLogConfig() = default;

  /**
   * Constructor from member values.
   */
  FrameLogConfig(FrameLogType type, std::string path, int fps, bool mount);
};

void to_json(nlohmann::json& j, const FrameLogConfig& l);
void from_json(const nlohmann::json& j, FrameLogConfig& l);

inline bool operator==(const FrameLogConfig& lhs, const FrameLogConfig& rhs) {
  return lhs.type == rhs.type && lhs.path == rhs.path && lhs.fps == rhs.fps &&
         lhs.mount == rhs.mount;
}

inline bool operator!=(const FrameLogConfig& lhs, const FrameLogConfig& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
