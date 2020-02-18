#include "filter_config.h"

using namespace deadeye;
using json = nlohmann::json;

char const* FilterConfig::kAreaKey{"area"};
char const* FilterConfig::kFullnessKey{"fullness"};
char const* FilterConfig::kAspectKey{"aspect"};

FilterConfig::FilterConfig(filter_t area, filter_t fullness, filter_t aspect)
    : area(area), fullness(fullness), aspect(aspect) {}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const FilterConfig& fc) {
  j = json{{FilterConfig::kAreaKey, fc.area},
           {FilterConfig::kFullnessKey, fc.fullness},
           {FilterConfig::kAspectKey, fc.aspect}};
}

void deadeye::from_json(const json& j, FilterConfig& fc) {
  j.at(FilterConfig::kAreaKey).get_to(fc.area);
  j.at(FilterConfig::kFullnessKey).get_to(fc.fullness);
  j.at(FilterConfig::kAspectKey).get_to(fc.aspect);
  fc.area_enabled = fc.area[0] != kAreaMin || fc.area[1] != kAreaMax;
  fc.fullness_enabled =
      fc.fullness[0] != kFullnessMin || fc.fullness[1] != kFullnessMax;
  fc.aspect_enabled = fc.aspect[0] != kAspectMin || fc.aspect[1] != kAspectMax;
  fc.enabled = fc.area_enabled || fc.fullness_enabled || fc.aspect_enabled;
}
