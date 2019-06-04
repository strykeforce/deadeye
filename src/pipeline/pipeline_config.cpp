#include "pipeline_config.hpp"

using namespace deadeye;
using json = nlohmann::json;

char const* PipelineConfig::kSerialKey{"sn"};
char const* PipelineConfig::kHsvHueKey{"hue"};
char const* PipelineConfig::kHsvSatKey{"sat"};
char const* PipelineConfig::kHsvValKey{"val"};
char const* PipelineConfig::kExposureKey{"exposure"};

PipelineConfig PipelineConfig::New(std::shared_ptr<nt::Value> value) {
  auto j = json::parse(value->GetString().str());
  return j.get<PipelineConfig>();
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const PipelineConfig& p) {
  j = json{{PipelineConfig::kSerialKey, p.sn},
           {PipelineConfig::kHsvHueKey, p.hue},
           {PipelineConfig::kHsvSatKey, p.sat},
           {PipelineConfig::kHsvValKey, p.val},
           {PipelineConfig::kExposureKey, p.exposure}};
}

void deadeye::from_json(const json& j, PipelineConfig& p) {
  j.at(PipelineConfig::kSerialKey).get_to(p.sn);
  j.at(PipelineConfig::kHsvHueKey).get_to(p.hue);
  j.at(PipelineConfig::kHsvSatKey).get_to(p.sat);
  j.at(PipelineConfig::kHsvValKey).get_to(p.val);
  j.at(PipelineConfig::kExposureKey).get_to(p.exposure);
}

// ---------------------------------------------------------------------------
// iostream support
//
namespace {
std::ostream& operator<<(std::ostream& os, std::array<int, 2> const& s) {
  os << "[" << s[0] << "," << s[1] << "]";
  return os;
}
}  // namespace

std::ostream& operator<<(std::ostream& os, PipelineConfig const& pc) {
  os << "PipelineConfig{" << pc.sn << ", " << pc.hue << ", " << pc.sat << ", "
     << pc.val << ", " << pc.exposure << "}";
  return os;
}
