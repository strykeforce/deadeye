#include "pipeline_config.h"

using namespace deadeye;
using json = nlohmann::json;

char const* PipelineConfig::kSerialKey{"sn"};
char const* PipelineConfig::kHsvHueKey{"hue"};
char const* PipelineConfig::kHsvSatKey{"sat"};
char const* PipelineConfig::kHsvValKey{"val"};
char const* PipelineConfig::kExposureKey{"exposure"};
char const* PipelineConfig::kCaptureConfigKey{"gstreamer"};

PipelineConfig::PipelineConfig()
    : sn(-1),
      hue({-1, -1}),
      sat({-1, -1}),
      val({-1, -1}),
      exposure(-1.0),
      capture_config() {}

PipelineConfig::PipelineConfig(int sn, hsv_t hue, hsv_t sat, hsv_t val,
                               double exposure, CaptureConfig capture_config)
    : sn(sn),
      hue(hue),
      sat(sat),
      val(val),
      exposure(exposure),
      capture_config(capture_config) {}

PipelineConfig::PipelineConfig(std::shared_ptr<nt::Value> value) {
  assert(value);
  auto j = json::parse(value->GetString().str());
  j.at(kSerialKey).get_to(sn);
  j.at(kHsvHueKey).get_to(hue);
  j.at(kHsvSatKey).get_to(sat);
  j.at(kHsvValKey).get_to(val);
  j.at(kExposureKey).get_to(exposure);
  j.at(kCaptureConfigKey).get_to(capture_config);
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
           {PipelineConfig::kCaptureConfigKey, p.capture_config}};
}

void deadeye::from_json(const json& j, PipelineConfig& p) {
  j.at(PipelineConfig::kSerialKey).get_to(p.sn);
  j.at(PipelineConfig::kHsvHueKey).get_to(p.hue);
  j.at(PipelineConfig::kHsvSatKey).get_to(p.sat);
  j.at(PipelineConfig::kHsvValKey).get_to(p.val);
  j.at(PipelineConfig::kExposureKey).get_to(p.exposure);
  j.at(PipelineConfig::kCaptureConfigKey).get_to(p.capture_config);
}
