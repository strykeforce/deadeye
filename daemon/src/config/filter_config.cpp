// Copyright (c) 2022 Stryke Force FRC 2767

#include "filter_config.h"

using ::deadeye::FilterConfig;
using json = nlohmann::json;

FilterConfig::FilterConfig(filter_t area, filter_t solidity, filter_t aspect)
    : area(area), solidity(solidity), aspect(aspect) {}

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
}
