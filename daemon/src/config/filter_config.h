#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace deadeye {

static constexpr double kAreaMin = 0.0;
static constexpr double kAreaMax = 1.0;
static constexpr double kSolidityMin = 0.0;
static constexpr double kSolidityMax = 1.0;
static constexpr double kAspectMin = 0.0;
static constexpr double kAspectMax = 20.0;

struct FilterConfig {
  static constexpr auto kAreaKey = "area";
  static constexpr auto kSolidityKey = "solidity";
  static constexpr auto kAspectKey = "aspect";

  using filter_t = std::array<double, 2>;

  filter_t area{kAreaMin, kAreaMax};
  filter_t solidity{kSolidityMin, kSolidityMax};
  filter_t aspect{kAspectMin, kAspectMax};

  int frame_area{0};

  /**
   * Default constructor.
   */
  FilterConfig() = default;

  /**
   * Constructor from member values.
   */
  FilterConfig(filter_t area, filter_t solidity, filter_t aspect);

  bool IsAreaEnabled() const {
    return area[0] != kAreaMin || area[1] != kAreaMax;
  }
  bool IsSolidityEnabled() const {
    return solidity[0] != kSolidityMin || solidity[1] != kSolidityMax;
  }
  bool IsAspectEnabled() const {
    return aspect[0] != kAspectMin || aspect[1] != kAspectMax;
  }
};

void to_json(nlohmann::json& j, const FilterConfig& l);
void from_json(const nlohmann::json& j, FilterConfig& l);

inline bool operator==(const FilterConfig& lhs, const FilterConfig& rhs) {
  return lhs.area == rhs.area && lhs.solidity == rhs.solidity &&
         lhs.aspect == rhs.aspect;
}

inline bool operator!=(const FilterConfig& lhs, const FilterConfig& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
