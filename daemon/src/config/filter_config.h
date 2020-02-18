#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {

struct FilterConfig {
  static char const* kAreaKey;
  static char const* kFullnessKey;
  static char const* kAspectKey;

  using filter_t = std::array<double, 2>;

  filter_t area{0.0, 100.0};
  filter_t fullness{0.0, 100.0};
  filter_t aspect{0.0, 100.0};

  /**
   * Default constructor.
   */
  FilterConfig() = default;

  /**
   * Constructor from member values.
   */
  FilterConfig(filter_t area, filter_t fullness, filter_t aspect);
};

void to_json(json& j, const FilterConfig& l);
void from_json(const json& j, FilterConfig& l);

inline bool operator==(FilterConfig const& lhs, FilterConfig const& rhs) {
  return lhs.area == rhs.area && lhs.fullness == rhs.fullness &&
         lhs.aspect == rhs.aspect;
}

inline bool operator!=(FilterConfig const& lhs, FilterConfig const& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
