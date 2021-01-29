#include "link/target_list_target_data.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <opencv2/imgproc.hpp>

using namespace deadeye;
using json = nlohmann::json;

namespace {
constexpr int MAX_SERIALIZED_SIZE = 510;
const cv::Scalar BB_COLOR{20, 255, 20};
}  // namespace

TargetListTargetData::TargetListTargetData(const std::string id, const int sn,
                                           const bool valid,
                                           const TargetList& targets)
    : TargetData{id, sn, valid}, targets{targets} {}

void TargetListTargetData::DrawMarkers(cv::Mat& preview) const {
  for (const auto& t : targets) {
    cv::Rect bb{t[0], t[1], t[2], t[3]};
    cv::rectangle(preview, bb, BB_COLOR, 2);
  }
}

std::string TargetListTargetData::Dump() const {
  json j = json{{TargetData::kIdKey, id},
                {TargetData::kSerialKey, serial},
                {TargetData::kValidKey, valid},
                {TargetData::kDataKey, targets}};

  auto serialized = j.dump();

  if (serialized.size() > MAX_SERIALIZED_SIZE) {
    spdlog::error("TargetListTargetData too big for UDP: {}",
                  serialized.size());
    TargetList targets{{{-1, -1, -1, -1, -1}}};
    TargetListTargetData td{id, serial, false, targets};
    return td.Dump();
  }

  return serialized;
}

std::string TargetListTargetData::ToString() const {
  return fmt::format("id={} sn={} val={})", id, serial, valid);
}
