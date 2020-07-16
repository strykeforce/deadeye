#include "target_data.h"

using namespace deadeye;
using json = nlohmann::json;

char const* TargetData::kIdKey{"id"};
char const* TargetData::kSerialKey{"sn"};
char const* TargetData::kValidKey{"valid"};

TargetData::TargetData(std::string id, int serial, bool valid)
    : id(id), serial(serial), valid(valid) {}

void TargetData::DrawMarkers(cv::Mat& preview) const {}

// Dump method here instead of normal nlohmann to_json() because we need to call
// this polymorphically.
std::string TargetData::Dump() const {
  json j = json{{TargetData::kIdKey, id},
                {TargetData::kSerialKey, serial},
                {TargetData::kValidKey, valid}};
  return j.dump();
}
