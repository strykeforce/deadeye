#pragma once
#include <nlohmann/json.hpp>
#include <string>

#include "log/log_type.h"

namespace deadeye {

struct LogConfig {
  static constexpr auto kTypeKey = "type";
  static constexpr auto kFpsKey = "fps";
  static constexpr auto kPathKey = "path";
  static constexpr auto kMountKey = "mount";
  static constexpr auto kDefaultPath = "/var/opt/deadeye";

  LogType type;
  std::string path{kDefaultPath};
  int fps{0};
  bool mount{true};

  /**
   * Default constructor.
   */
  LogConfig() = default;

  /**
   * Constructor from member values.
   */
  LogConfig(LogType type, std::string path, int fps, bool mount);
};

void to_json(nlohmann::json& j, const LogConfig& l);
void from_json(const nlohmann::json& j, LogConfig& l);

inline bool operator==(const LogConfig& lhs, const LogConfig& rhs) {
  return lhs.type == rhs.type && lhs.path == rhs.path && lhs.fps == rhs.fps &&
         lhs.mount == rhs.mount;
}

inline bool operator!=(const LogConfig& lhs, const LogConfig& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
