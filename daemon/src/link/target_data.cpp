#include "target_data.hpp"

using namespace deadeye;
using json = nlohmann::json;

char const* TargetData::kIdKey{"id"};
char const* TargetData::kSerialKey{"sn"};
char const* TargetData::kValidKey{"valid"};

TargetData::TargetData() {}
TargetData::TargetData(std::string id, int serial, bool valid)
    : id(id), serial(serial), valid(valid) {}

void TargetData::DrawMarkers(cv::Mat& preview) {}

std::string TargetData::Dump() const {
  json j = json{{TargetData::kIdKey, id},
                {TargetData::kSerialKey, serial},
                {TargetData::kValidKey, valid}};
  return j.dump();
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, TargetData const& td) {
  j = json{{TargetData::kSerialKey, td.serial},
           {TargetData::kValidKey, td.valid}};
}

void deadeye::from_json(json const& j, TargetData& td) {
  throw new std::runtime_error("JSON parsing not supported");
}
