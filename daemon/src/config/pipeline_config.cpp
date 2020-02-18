#include "pipeline_config.h"

using namespace deadeye;
using json = nlohmann::json;

char const* PipelineConfig::kSerialKey{"sn"};
char const* PipelineConfig::kHsvHueKey{"hue"};
char const* PipelineConfig::kHsvSatKey{"sat"};
char const* PipelineConfig::kHsvValKey{"val"};
char const* PipelineConfig::kExposureKey{"exposure"};
char const* PipelineConfig::kFilterKey{"filter"};
char const* PipelineConfig::kLogKey{"log"};

PipelineConfig::PipelineConfig(int sn, hsv_t hue, hsv_t sat, hsv_t val,
                               double exposure, FilterConfig filter,
                               LogConfig log)
    : sn(sn),
      hue(hue),
      sat(sat),
      val(val),
      exposure(exposure),
      filter(filter),
      log(log) {}

PipelineConfig::PipelineConfig(std::shared_ptr<nt::Value> value) {
  assert(value);
  auto j = json::parse(value->GetString().str());
  j.at(kSerialKey).get_to(sn);
  j.at(kHsvHueKey).get_to(hue);
  j.at(kHsvSatKey).get_to(sat);
  j.at(kHsvValKey).get_to(val);
  j.at(kExposureKey).get_to(exposure);
  if (j.contains(PipelineConfig::kFilterKey))
    j.at(PipelineConfig::kFilterKey).get_to(filter);
  if (j.contains(PipelineConfig::kLogKey))
    j.at(PipelineConfig::kLogKey).get_to(log);
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const PipelineConfig& p) {
  j = json{{PipelineConfig::kSerialKey, p.sn},
           {PipelineConfig::kHsvHueKey, p.hue},
           {PipelineConfig::kHsvSatKey, p.sat},
           {PipelineConfig::kHsvValKey, p.val},
           {PipelineConfig::kExposureKey, p.exposure},
           {PipelineConfig::kFilterKey, p.filter},
           {PipelineConfig::kLogKey, p.log}};
}

void deadeye::from_json(const json& j, PipelineConfig& p) {
  j.at(PipelineConfig::kSerialKey).get_to(p.sn);
  j.at(PipelineConfig::kHsvHueKey).get_to(p.hue);
  j.at(PipelineConfig::kHsvSatKey).get_to(p.sat);
  j.at(PipelineConfig::kHsvValKey).get_to(p.val);
  j.at(PipelineConfig::kExposureKey).get_to(p.exposure);
  if (j.contains(PipelineConfig::kFilterKey))
    j.at(PipelineConfig::kFilterKey).get_to(p.filter);
  if (j.contains(PipelineConfig::kLogKey))
    j.at(PipelineConfig::kLogKey).get_to(p.log);
}
