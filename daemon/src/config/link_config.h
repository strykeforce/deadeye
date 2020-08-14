#pragma once
#include <networktables/NetworkTableValue.h>

#include <nlohmann/json.hpp>
#include <string>

namespace deadeye {

struct LinkConfig {
  static constexpr auto kAddressKey = "address";
  static constexpr auto kPortKey = "port";
  static constexpr auto kEnabledKey = "enabled";

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
  LinkConfig(const nlohmann::json& j);
};

void to_json(nlohmann::json& j, const LinkConfig& l);
void from_json(const nlohmann::json& j, LinkConfig& l);

inline bool operator==(const LinkConfig& lhs, const LinkConfig& rhs) {
  return lhs.address == rhs.address && lhs.port == rhs.port &&
         lhs.enabled == rhs.enabled;
}

inline bool operator!=(const LinkConfig& lhs, const LinkConfig& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
