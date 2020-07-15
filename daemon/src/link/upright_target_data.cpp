#include "upright_target_data.h"

#include <fmt/core.h>

#include <opencv2/imgproc.hpp>

using namespace deadeye;
using json = nlohmann::json;

char const* UprightTargetData::kTLX{"tx"};
char const* UprightTargetData::kTLY{"ty"};
char const* UprightTargetData::kBRX{"bx"};
char const* UprightTargetData::kBRY{"by"};
char const* UprightTargetData::kXKey{"x"};
char const* UprightTargetData::kYKey{"y"};

UprightTargetData::UprightTargetData(std::string id, int sn, bool valid,
                                     cv::Rect bb, cv::Point center)
    : TargetData{id, sn, valid}, bb(bb) {
  cv::Point target = (bb.tl() + bb.br()) / 2;
  offset = target - center;
}

void UprightTargetData::DrawMarkers(cv::Mat& preview) const {
  cv::Point center{preview.cols / 2, preview.rows / 2};
  cv::Point target = center + offset;
  cv::drawMarker(preview, target, cv::Scalar::all(255));
  cv::rectangle(preview, bb, cv::Scalar(255, 0, 0));
}

std::string UprightTargetData::Dump() const {
  json j = json{{TargetData::kIdKey, id},
                {TargetData::kSerialKey, serial},
                {TargetData::kValidKey, valid},
                {UprightTargetData::kTLX, bb.tl().x},
                {UprightTargetData::kTLY, bb.tl().y},
                {UprightTargetData::kBRX, bb.br().x},
                {UprightTargetData::kBRY, bb.br().y},
                {UprightTargetData::kXKey, offset.x},
                {UprightTargetData::kYKey, offset.y}};
  return j.dump();
}

std::string UprightTargetData::ToString() const {
  return fmt::format("id={} sn={} val={} tl=({},{}) br=({},{}) off=({},{})", id,
                     serial, valid, bb.tl().x, bb.tl().y, bb.br().x, bb.br().y,
                     offset.x, offset.y);
}
