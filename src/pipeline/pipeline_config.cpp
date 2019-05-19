#include "pipeline_config.hpp"

using namespace deadeye;
using json = nlohmann::json;

char const* PipelineConfig::kSerialKey{"sn"};
char const* PipelineConfig::kHsvLowKey{"hsv_low"};
char const* PipelineConfig::kHsvHighKey{"hsv_high"};
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
           {PipelineConfig::kHsvLowKey, p.hsv_low},
           {PipelineConfig::kHsvHighKey, p.hsv_high},
           {PipelineConfig::kExposureKey, p.exposure}};
}

void deadeye::from_json(const json& j, PipelineConfig& p) {
  j.at(PipelineConfig::kSerialKey).get_to(p.sn);
  j.at(PipelineConfig::kHsvLowKey).get_to(p.hsv_low);
  j.at(PipelineConfig::kHsvHighKey).get_to(p.hsv_high);
  j.at(PipelineConfig::kExposureKey).get_to(p.exposure);
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
