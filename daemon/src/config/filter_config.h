#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {

namespace {
static constexpr double kAreaMin = 0.0;
static constexpr double kAreaMax = 1.0;
static constexpr double kSolidityMin = 0.0;
static constexpr double kSolidityMax = 1.0;
static constexpr double kAspectMin = 0.0;
static constexpr double kAspectMax = 20.0;
}  // namespace

struct FilterConfig {
  static const char* kAreaKey;
  static const char* kSolidityKey;
  static const char* kAspectKey;

  using filter_t = std::array<double, 2>;

  filter_t area{kAreaMin, kAreaMax};
  filter_t solidity{kSolidityMin, kSolidityMax};
  filter_t aspect{kAspectMin, kAspectMax};

  int frame_area{0};

  bool enabled = false;
  bool area_enabled = false;
  bool solidity_enabled = false;
  bool aspect_enabled = false;

  /**
   * Default constructor.
   */
  FilterConfig() = default;

  /**
   * Constructor from member values.
   */
  FilterConfig(filter_t area, filter_t solidity, filter_t aspect);
};

void to_json(json& j, const FilterConfig& l);
void from_json(const json& j, FilterConfig& l);

inline bool operator==(const FilterConfig& lhs, const FilterConfig& rhs) {
  return lhs.area == rhs.area && lhs.solidity == rhs.solidity &&
         lhs.aspect == rhs.aspect;
}

inline bool operator!=(const FilterConfig& lhs, const FilterConfig& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
