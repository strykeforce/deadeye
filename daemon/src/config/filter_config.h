#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {

namespace {
static constexpr double kAreaMin = 0.0;
static constexpr double kAreaMax = 1.0;
static constexpr double kFullnessMin = 0.0;
static constexpr double kFullnessMax = 1.0;
static constexpr double kAspectMin = 0.0;
static constexpr double kAspectMax = 20.0;
}  // namespace

struct FilterConfig {
  static char const* kAreaKey;
  static char const* kFullnessKey;
  static char const* kAspectKey;

  using filter_t = std::array<double, 2>;

  filter_t area{kAreaMin, kAreaMax};
  filter_t fullness{kFullnessMin, kFullnessMax};
  filter_t aspect{kAspectMin, kAspectMax};

  bool enabled = false;
  bool area_enabled = false;
  bool fullness_enabled = false;
  bool aspect_enabled = false;

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
