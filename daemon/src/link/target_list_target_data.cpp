#include "link/target_list_target_data.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <opencv2/imgproc.hpp>
#include <utility>

using namespace deadeye;
using json = nlohmann::json;

namespace {
// minimum datagram size: IPv4 = 576 IPv6 = 1280
constexpr int MAX_SERIALIZED_SIZE = 1000;
const cv::Scalar BB_COLOR{20, 255, 20};  // NOLINT
}  // namespace

TargetListTargetData::TargetListTargetData(const std::string_view id,
                                           const int sn, const bool valid,
                                           TargetList targets)
    : TargetData{id, sn, valid}, targets{std::move(targets)} {}

void TargetListTargetData::DrawMarkers(cv::Mat& preview) const {
  for (const auto& t : targets) {
    cv::Rect bb{t[0], t[1], t[2], t[3]};
    cv::rectangle(preview, bb, BB_COLOR, 2);
  }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
std::string TargetListTargetData::Dump() const {
  json j = json{{TargetData::kIdKey, id},
                {TargetData::kSerialKey, serial},
                {TargetData::kValidKey, valid},
                {TargetData::kDataKey, targets}};

  auto serialized = j.dump();

  if (serialized.size() > MAX_SERIALIZED_SIZE) {
    spdlog::error("TargetListTargetData too big for UDP: {}",
                  serialized.size());
    TargetList error_targets{{{-1, -1, -1, -1, -1}}};
    TargetListTargetData td{id, serial, false, error_targets};
    return td.Dump();
  }

  return serialized;
}
#pragma clang diagnostic pop

std::string TargetListTargetData::ToString() const {
  return fmt::format("id={} sn={} val={})", id, serial, valid);
}
