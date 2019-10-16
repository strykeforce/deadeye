#pragma once
#include <networktables/NetworkTableValue.h>
#include <spdlog/fmt/ostr.h>
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
   * Default constructor.
   */
  StreamConfig();

  /**
   * Construct with default values for camera inum.
   */
  StreamConfig(int inum);

  /**
   * Constructor from member values.
   */
  StreamConfig(int sn, std::string url, View view, Contour contour);

  /**
   * Constructor from NetworkTables.
   */
  StreamConfig(std::shared_ptr<nt::Value> value);

  template <typename OStream>
  friend OStream& operator<<(OStream& os, StreamConfig const& sc) {
    std::string view;
    switch (sc.view) {
      case View::NONE:
        view = "NONE";
        break;
      case View::ORIGINAL:
        view = "ORIGINAL";
        break;
      case View::MASK:
        view = "MASK";
        break;
    }

    std::string contour;
    switch (sc.contour) {
      case Contour::NONE:
        contour = "NONE";
        break;
      case Contour::FILTER:
        contour = "FILTER";
        break;
      case Contour::ALL:
        contour = "ALL";
        break;
    }
    os << "StreamConfig{sn=" << sc.sn << ", url=\"" << sc.url
       << "\", view=" << view << ", contour=" << contour << "}";
    return os;
  }
};

void to_json(json& j, const StreamConfig& p);
void from_json(const json& j, StreamConfig& p);

inline bool operator==(StreamConfig const& lhs, StreamConfig const& rhs) {
  return lhs.view == rhs.view && lhs.contour == rhs.contour &&
         lhs.url == rhs.url;
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