#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {

struct LogConfig {
  static const char* kFpsKey;
  static const char* kPathKey;
  static const char* kMountKey;
  static const char* kDefaultPath;

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

inline bool operator==(const LogConfig& lhs, const LogConfig& rhs) {
  return lhs.path == rhs.path && lhs.fps == rhs.fps && lhs.mount == rhs.mount;
}

inline bool operator!=(const LogConfig& lhs, const LogConfig& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
