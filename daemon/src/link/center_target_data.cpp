#include "center_target_data.h"

#include <fmt/core.h>
#include <opencv2/imgproc.hpp>

using namespace deadeye;
using json = nlohmann::json;

char const* CenterTargetData::kXKey{"x"};
char const* CenterTargetData::kYKey{"y"};

CenterTargetData::CenterTargetData(std::string id, int sn, bool valid,
                                   cv::Rect bb, cv::Point center)
    : TargetData{id, sn, valid}, bb(bb) {
  cv::Point target = (bb.tl() + bb.br()) / 2;
  offset = target - center;
}

void CenterTargetData::DrawMarkers(cv::Mat& preview) {
  cv::Point center{preview.cols / 2, preview.rows / 2};
  cv::Point target = center + offset;
  cv::drawMarker(preview, target, cv::Scalar::all(255));
}

std::string CenterTargetData::Dump() const {
  json j = json{{TargetData::kIdKey, id},
                {TargetData::kSerialKey, serial},
                {TargetData::kValidKey, valid},
                {CenterTargetData::kXKey, offset.x},
                {CenterTargetData::kYKey, offset.y}};
  return j.dump();
}

std::string CenterTargetData::ToString() const {
  return fmt::format("id={} sn={} valid={} x={} y={}", id, serial, valid,
                     offset.x, offset.y);
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, CenterTargetData const& ctd) {
  j = json{{TargetData::kSerialKey, ctd.serial},
           {TargetData::kValidKey, ctd.valid},
           {CenterTargetData::kXKey, ctd.offset.x},
           {CenterTargetData::kYKey, ctd.offset.y}};
}

void deadeye::from_json(json const& j, CenterTargetData& ctd) {
  throw new std::runtime_error("JSON parsing not supported");
}
