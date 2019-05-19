#include "pipeline_config.hpp"

using namespace deadeye;
using json = nlohmann::json;

PipelineConfig PipelineConfig::New(std::shared_ptr<nt::Value> value) {
  auto j = json::parse(value->GetString().str());
  return j.get<PipelineConfig>();
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const PipelineConfig& p) {
  j = json{{kSerialKey, p.sn},
           {kHsvLowKey, p.hsv_low},
           {kHsvHighKey, p.hsv_high},
           {kExposureKey, p.exposure}};
}

void deadeye::from_json(const json& j, PipelineConfig& p) {
  j.at(kSerialKey).get_to(p.sn);
  j.at(kHsvLowKey).get_to(p.hsv_low);
  j.at(kHsvHighKey).get_to(p.hsv_high);
  j.at(kExposureKey).get_to(p.exposure);
}

// ---------------------------------------------------------------------------
// iostream support
//
namespace {
std::ostream& operator<<(std::ostream& os, std::array<int, 3> const& s) {
  os << "[" << s[0] << "," << s[1] << "," << s[2] << "]";
  return os;
}
}  // namespace

std::ostream& operator<<(std::ostream& os, PipelineConfig const& pc) {
  os << "PipelineConfig{" << pc.sn << ", " << pc.hsv_low << ", " << pc.hsv_high
     << ", " << pc.exposure << "}";
  return os;
}
