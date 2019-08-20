#pragma once
#include <networktables/NetworkTableValue.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {

struct StreamConfig {
  enum class View { NONE, ORIGINAL, MASK };
  enum class Contour { NONE, FILTER, ALL };

  static char const* kSerialKey;
  static char const* kUrlKey;
  static char const* kViewKey;
  static char const* kContourKey;

  int sn = 0;
  std::string url;
  View view = View::NONE;
  Contour contour = Contour::NONE;

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

NLOHMANN_JSON_SERIALIZE_ENUM(StreamConfig::View,
                             {
                                 {StreamConfig::View::NONE, "none"},
                                 {StreamConfig::View::ORIGINAL, "original"},
                                 {StreamConfig::View::MASK, "mask"},
                             })

NLOHMANN_JSON_SERIALIZE_ENUM(StreamConfig::Contour,
                             {
                                 {StreamConfig::Contour::NONE, "none"},
                                 {StreamConfig::Contour::FILTER, "filter"},
                                 {StreamConfig::Contour::ALL, "all"},
                             })

}  // namespace deadeye

std::ostream& operator<<(std::ostream& os, deadeye::StreamConfig const& pc);
