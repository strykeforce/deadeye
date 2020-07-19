#include "pipeline_config.h"

using namespace deadeye;
using json = nlohmann::json;

const char* PipelineConfig::kSerialKey{"sn"};
const char* PipelineConfig::kHsvHueKey{"hue"};
const char* PipelineConfig::kHsvSatKey{"sat"};
const char* PipelineConfig::kHsvValKey{"val"};
const char* PipelineConfig::kFilterKey{"filter"};
const char* PipelineConfig::kLogKey{"log"};

PipelineConfig::PipelineConfig(int sn, hsv_t hue, hsv_t sat, hsv_t val,
                               FilterConfig filter, LogConfig log)
    : sn(sn), hue(hue), sat(sat), val(val), filter(filter), log(log) {}

PipelineConfig::PipelineConfig(std::shared_ptr<nt::Value> value) {
  assert(value);
  auto j = json::parse(value->GetString().str());
  j.at(kSerialKey).get_to(sn);
  j.at(kHsvHueKey).get_to(hue);
  j.at(kHsvSatKey).get_to(sat);
  j.at(kHsvValKey).get_to(val);
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
           {PipelineConfig::kFilterKey, p.filter},
           {PipelineConfig::kLogKey, p.log}};
}

void deadeye::from_json(const json& j, PipelineConfig& p) {
  j.at(PipelineConfig::kSerialKey).get_to(p.sn);
  j.at(PipelineConfig::kHsvHueKey).get_to(p.hue);
  j.at(PipelineConfig::kHsvSatKey).get_to(p.sat);
  j.at(PipelineConfig::kHsvValKey).get_to(p.val);
  if (j.contains(PipelineConfig::kFilterKey))
    j.at(PipelineConfig::kFilterKey).get_to(p.filter);
  if (j.contains(PipelineConfig::kLogKey))
    j.at(PipelineConfig::kLogKey).get_to(p.log);
}
