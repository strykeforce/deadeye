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
  rect.points(corners);
}

void MinAreaTargetData::DrawMarkers(cv::Mat& preview) {
  cv::Point center{preview.cols / 2, preview.rows / 2};
  cv::Point2f target = static_cast<cv::Point2f>(center) + offset;
  cv::drawMarker(preview, target, cv::Scalar::all(255));
  for (int i = 0; i < 4; i++)
    cv::circle(preview, corners[i], 3, cv::Scalar(0, 0, 255), cv::FILLED,
               cv::LINE_AA);
  // cv::rectangle(preview, bb, cv::Scalar(255, 0, 0));
}

std::string MinAreaTargetData::Dump() const {
  json j = json{{TargetData::kIdKey, id},
                {TargetData::kSerialKey, serial},
                {TargetData::kValidKey, valid},
                {MinAreaTargetData::kBLX, corners[0].x},
                {MinAreaTargetData::kBLY, corners[0].y},
                {MinAreaTargetData::kTLX, corners[1].x},
                {MinAreaTargetData::kTLY, corners[1].y},
                {MinAreaTargetData::kTRX, corners[2].x},
                {MinAreaTargetData::kTRY, corners[2].y},
                {MinAreaTargetData::kBRX, corners[3].x},
                {MinAreaTargetData::kBRY, corners[3].y},
                {MinAreaTargetData::kXKey, offset.x},
                {MinAreaTargetData::kYKey, offset.y},
                {MinAreaTargetData::kWKey, rect.size.width},
                {MinAreaTargetData::kHKey, rect.size.height},
                {MinAreaTargetData::kAngleKey, rect.angle}};
  return j.dump();
}

std::string MinAreaTargetData::ToString() const {
  return fmt::format(
      "id={} sn={} val={} bl=({:.1f},{:.1f}) tl=({:.1f},{:.1f}) "
      "tr=({:.1f},{:.1f}) br=({:.1f},{:.1f}) "
      "off=({:.1f},{:.1f}) w={:.1f} h={:.1f} a={:.1f}",
      id, serial, valid, corners[0].x, corners[0].y, corners[1].x, corners[1].y,
      corners[2].x, corners[2].y, corners[3].x, corners[3].y, offset.x,
      offset.y, rect.size.width, rect.size.height, rect.angle);
}
