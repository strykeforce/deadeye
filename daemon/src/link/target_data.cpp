// Copyright (c) 2022 Stryke Force FRC 2767

#include "target_data.h"

#include <string>
#include <utility>

using ::deadeye::TargetData;
using json = nlohmann::json;

const char* TargetData::kIdKey{"id"};
const char* TargetData::kSerialKey{"sn"};
const char* TargetData::kValidKey{"v"};
const char* TargetData::kDataKey{"d"};

TargetData::TargetData(std::string_view id, int serial, bool valid)
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
