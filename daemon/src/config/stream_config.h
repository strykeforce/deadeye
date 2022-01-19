#pragma once
#include <networktables/NetworkTableValue.h>
#include <spdlog/fmt/ostr.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

namespace deadeye {

struct StreamConfig {
  enum class View { none, original, mask };
  enum class Contour { none, filter, all };

  static constexpr auto kSerialKey = "sn";
  static constexpr auto kUrlKey = "url";
  static constexpr auto kViewKey = "view";
  static constexpr auto kContourKey = "contour";

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
  explicit StreamConfig(int inum);

  /**
   * Constructor from member values.
   */
  StreamConfig(int sn, std::string url, View view, Contour contour);

  /**
   * Constructor from NetworkTables.
   */
  explicit StreamConfig(const std::shared_ptr<nt::Value>& value);

  /**
   * Get stream enabled.
   */
  [[nodiscard]] inline bool StreamEnabled() const {
    return !(view == View::none && contour == Contour::none);
  }

  template <typename OStream>
  friend OStream& operator<<(OStream& os, const StreamConfig& sc) {
    std::string current_view;
    switch (sc.view) {
      case View::none:
        current_view = "none";
        break;
      case View::original:
        current_view = "original";
        break;
      case View::mask:
        current_view = "mask";
        break;
    }

    std::string current_contour;
    switch (sc.contour) {
      case Contour::none:
        current_contour = "none";
        break;
      case Contour::filter:
        current_contour = "filter";
        break;
      case Contour::all:
        current_contour = "ALL";
        break;
    }
    os << "StreamConfig<sn=" << sc.sn << ", url=\"" << sc.url
       << "\", view=" << current_view << ", contour=" << current_contour << ">";
    return os;
  }
};

void to_json(nlohmann::json& j, const StreamConfig& p);
void from_json(const nlohmann::json& j, StreamConfig& p);

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
