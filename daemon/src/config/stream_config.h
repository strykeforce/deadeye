#pragma once
#include <networktables/NetworkTableValue.h>
#include <spdlog/fmt/ostr.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {

struct StreamConfig {
  enum class View { none, original, mask };
  enum class Contour { none, filter, all };

  static const char* kSerialKey;
  static const char* kUrlKey;
  static const char* kViewKey;
  static const char* kContourKey;

  int sn = 0;
  std::string url;
  View view;
  Contour contour;

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

  /**
   * Get stream enabled.
   */
  inline bool StreamEnabled() const {
    return !(view == View::none && contour == Contour::none);
  }

  template <typename OStream>
  friend OStream& operator<<(OStream& os, const StreamConfig& sc) {
    std::string view;
    switch (sc.view) {
      case View::none:
        view = "none";
        break;
      case View::original:
        view = "original";
        break;
      case View::mask:
        view = "mask";
        break;
    }

    std::string contour;
    switch (sc.contour) {
      case Contour::none:
        contour = "none";
        break;
      case Contour::filter:
        contour = "filter";
        break;
      case Contour::all:
        contour = "ALL";
        break;
    }
    os << "StreamConfig<sn=" << sc.sn << ", url=\"" << sc.url
       << "\", view=" << view << ", contour=" << contour << ">";
    return os;
  }
};

void to_json(json& j, const StreamConfig& p);
void from_json(const json& j, StreamConfig& p);

inline bool operator==(const StreamConfig& lhs, const StreamConfig& rhs) {
  return lhs.view == rhs.view && lhs.contour == rhs.contour &&
         lhs.url == rhs.url;
}

inline bool operator!=(const StreamConfig& lhs, const StreamConfig& rhs) {
  return !(lhs == rhs);
}

NLOHMANN_JSON_SERIALIZE_ENUM(StreamConfig::View,
                             {
                                 {StreamConfig::View::none, "none"},
                                 {StreamConfig::View::original, "original"},
                                 {StreamConfig::View::mask, "mask"},
                             })

NLOHMANN_JSON_SERIALIZE_ENUM(StreamConfig::Contour,
                             {
                                 {StreamConfig::Contour::none, "none"},
                                 {StreamConfig::Contour::filter, "filter"},
                                 {StreamConfig::Contour::all, "all"},
                             })

}  // namespace deadeye
