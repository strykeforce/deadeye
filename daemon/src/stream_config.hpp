#pragma once
#include <networktables/NetworkTableValue.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {

struct StreamConfig {
  static char const* kSerialKey;
  static char const* kUrlKey;

  int sn = 0;
  std::string url;

  /**
   * New is factory method to create a StreamConfig from a NT value.
   */
  static StreamConfig New(std::shared_ptr<nt::Value> value);
};

void to_json(json& j, const StreamConfig& p);
void from_json(const json& j, StreamConfig& p);

inline bool operator==(StreamConfig const& lhs, StreamConfig const& rhs) {
  return lhs.url == rhs.url;
}

inline bool operator!=(StreamConfig const& lhs, StreamConfig const& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye

std::ostream& operator<<(std::ostream& os, deadeye::StreamConfig const& pc);
