#pragma once
#include <networktables/NetworkTableValue.h>

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {

struct LinkConfig {
  static const char* kAddressKey;
  static const char* kPortKey;
  static const char* kEnabledKey;

  std::string address;
  int port;
  bool enabled;

  /**
   * Default constructor.
   */
  LinkConfig();

  /**
   * Constructor from member values.
   */
  LinkConfig(std::string address, int port, bool enabled);

  /**
   * Constructor from NetworkTables value.
   */
  LinkConfig(std::shared_ptr<nt::Value> value);
};

void to_json(json& j, const LinkConfig& l);
void from_json(const json& j, LinkConfig& l);

inline bool operator==(const LinkConfig& lhs, const LinkConfig& rhs) {
  return lhs.address == rhs.address && lhs.port == rhs.port &&
         lhs.enabled == rhs.enabled;
}

inline bool operator!=(const LinkConfig& lhs, const LinkConfig& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
