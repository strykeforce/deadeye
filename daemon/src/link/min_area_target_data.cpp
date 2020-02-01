#include "min_area_target_data.h"

#include <fmt/core.h>
#include <opencv2/imgproc.hpp>

using namespace deadeye;
using json = nlohmann::json;

char const* MinAreaTargetData::kBLX{"blx"};
char const* MinAreaTargetData::kBLY{"bly"};
char const* MinAreaTargetData::kTLX{"tlx"};
char const* MinAreaTargetData::kTLY{"tly"};
char const* MinAreaTargetData::kTRX{"trx"};
char const* MinAreaTargetData::kTRY{"try"};
char const* MinAreaTargetData::kBRX{"brx"};
char const* MinAreaTargetData::kBRY{"bry"};
char const* MinAreaTargetData::kXKey{"x"};
char const* MinAreaTargetData::kYKey{"y"};
char const* MinAreaTargetData::kWKey{"w"};
char const* MinAreaTargetData::kHKey{"h"};
char const* MinAreaTargetData::kAngleKey{"a"};

MinAreaTargetData::MinAreaTargetData(std::string id, int sn, bool valid,
                                     cv::RotatedRect rect, cv::Point2f center)
    : TargetData{id, sn, valid}, rect(rect) {
  offset = rect.center - center;
}

void MinAreaTargetData::DrawMarkers(cv::Mat& preview) {
  cv::Point center{preview.cols / 2, preview.rows / 2};
  cv::Point2f target = static_cast<cv::Point2f>(center) + offset;
  cv::drawMarker(preview, target, cv::Scalar::all(255));
  // cv::rectangle(preview, bb, cv::Scalar(255, 0, 0));
}

std::string MinAreaTargetData::Dump() const {
  json j = json{{TargetData::kIdKey, id},
                {TargetData::kSerialKey, serial},
                {TargetData::kValidKey, valid},
                // {MinAreaTargetData::kTLX, bb.tl().x},
                // {MinAreaTargetData::kTLY, bb.tl().y},
                // {MinAreaTargetData::kBRX, bb.br().x},
                // {MinAreaTargetData::kBRY, bb.br().y},
                {MinAreaTargetData::kXKey, offset.x},
                {MinAreaTargetData::kYKey, offset.y}};
  return j.dump();
}

std::string MinAreaTargetData::ToString() const {
  return fmt::format(
      "id={} sn={} val={} tl=({},{}) br=({},{}) off=({},{})", id,
      //  serial, valid, bb.tl().x, bb.tl().y, bb.br().x, bb.br().y,
      offset.x, offset.y);
}
