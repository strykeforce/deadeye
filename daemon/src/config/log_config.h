#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {

struct LogConfig {
  static char const* kFpsKey;
  static char const* kPathKey;
  static char const* kMountKey;
  static char const* kDefaultPath;

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
  LogConfig(std::string path, int fps, bool mount);
};

void to_json(json& j, const LogConfig& l);
void from_json(const json& j, LogConfig& l);

inline bool operator==(LogConfig const& lhs, LogConfig const& rhs) {
  return lhs.path == rhs.path && lhs.fps == rhs.fps && lhs.mount == rhs.mount;
}

inline bool operator!=(LogConfig const& lhs, LogConfig const& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
