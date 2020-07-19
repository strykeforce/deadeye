#include "filter_config.h"

using namespace deadeye;
using json = nlohmann::json;

const char* FilterConfig::kAreaKey{"area"};
const char* FilterConfig::kSolidityKey{"solidity"};
const char* FilterConfig::kAspectKey{"aspect"};

FilterConfig::FilterConfig(filter_t area, filter_t solidity, filter_t aspect)
    : area(area), solidity(solidity), aspect(aspect) {
  area_enabled = area[0] != kAreaMin || area[1] != kAreaMax;
  solidity_enabled = solidity[0] != kSolidityMin || solidity[1] != kSolidityMax;
  aspect_enabled = aspect[0] != kAspectMin || aspect[1] != kAspectMax;
  enabled = area_enabled || solidity_enabled || aspect_enabled;
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const FilterConfig& fc) {
  j = json{{FilterConfig::kAreaKey, fc.area},
           {FilterConfig::kSolidityKey, fc.solidity},
           {FilterConfig::kAspectKey, fc.aspect}};
}

void deadeye::from_json(const json& j, FilterConfig& fc) {
  j.at(FilterConfig::kAreaKey).get_to(fc.area);
  j.at(FilterConfig::kSolidityKey).get_to(fc.solidity);
  j.at(FilterConfig::kAspectKey).get_to(fc.aspect);
  fc.area_enabled = fc.area[0] != kAreaMin || fc.area[1] != kAreaMax;
  fc.solidity_enabled =
      fc.solidity[0] != kSolidityMin || fc.solidity[1] != kSolidityMax;
  fc.aspect_enabled = fc.aspect[0] != kAspectMin || fc.aspect[1] != kAspectMax;
  fc.enabled = fc.area_enabled || fc.solidity_enabled || fc.aspect_enabled;
}
