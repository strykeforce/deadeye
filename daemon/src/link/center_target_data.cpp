#include "center_target_data.hpp"

#include <opencv2/imgproc.hpp>

using namespace deadeye;
using json = nlohmann::json;

char const* CenterTargetData::kXKey{"x"};
char const* CenterTargetData::kYKey{"y"};

CenterTargetData::CenterTargetData() {}
CenterTargetData::CenterTargetData(std::string id, int sn, bool valid, int x,
                                   int y)
    : TargetData{id, sn, valid}, x(x), y(y) {}

void CenterTargetData::DrawMarkers(cv::Mat& preview) {
  cv::drawMarker(preview, cv::Point(x, y), cv::Scalar(255, 255, 255));
}

std::string CenterTargetData::Dump() const {
  json j = json{{TargetData::kIdKey, id},
                {TargetData::kSerialKey, serial},
                {TargetData::kValidKey, valid},
                {CenterTargetData::kXKey, x},
                {CenterTargetData::kYKey, y}};
  return j.dump();
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, CenterTargetData const& ctd) {
  j = json{{TargetData::kSerialKey, ctd.serial},
           {TargetData::kValidKey, ctd.valid},
           {CenterTargetData::kXKey, ctd.x},
           {CenterTargetData::kYKey, ctd.y}};
}

void deadeye::from_json(json const& j, CenterTargetData& ctd) {
  throw new std::runtime_error("JSON parsing not supported");
}
