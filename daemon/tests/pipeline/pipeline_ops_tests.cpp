#include "pipeline_tests.h"

using namespace deadeye;

TEST_CASE("FindContours", "[ops]") {
  cv::Mat frame = cv::imread(kTargetMaster);
  REQUIRE(frame.size() == cv::Size(1280, 720));
  cv::Mat mask;
  Contours contours;

  SECTION("default hsv range") {
    PipelineConfig pc;
    MaskFrame(frame, mask, pc.HsvLow(), pc.HsvHigh());
    FindContours(mask, contours);
    REQUIRE(contours.empty());
  }

  SECTION("tuned hsv range") {
    PipelineConfig pc{kTargetMasterPipelineConfig};
    MaskFrame(frame, mask, pc.HsvLow(), pc.HsvHigh());
    FindContours(mask, contours);
    REQUIRE(contours.size() == kTargetMasterNumContours);
  }
}

void MaskWrite(cv::Mat const& mask, Contours const& contours) {
  cv::Mat output;
  cv::cvtColor(mask, output, cv::COLOR_GRAY2BGR);
  cv::drawContours(output, contours, -1, cv::Scalar(255, 0, 240), 2);
  cv::imwrite("mask.jpg", output);
}

TEST_CASE("area filter", "[ops]") {
  cv::Mat frame{cv::imread(kAreaFilter)};
  cv::Mat mask;
  MaskFrame(frame, mask, kHsvLow, kHsvHigh);

  Contours contours;
  FindContours(mask, contours);
  REQUIRE(contours.size() == 15);

  Contours filtered_contours;

  SECTION("no filters") {
    FilterConfig filter;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == contours.size());
  }

  SECTION("area 0.2713") {
    FilterConfig filter{
        {0.27, 0.29}, {kSolidityMin, kSolidityMax}, {kAspectMin, kAspectMax}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 1);
  }

  SECTION("area 0.0678") {
    FilterConfig filter{
        {0.06, 0.08}, {kSolidityMin, kSolidityMax}, {kAspectMin, kAspectMax}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 2);
  }

  SECTION("area 0.0170") {
    FilterConfig filter{
        {0.01, 0.02}, {kSolidityMin, kSolidityMax}, {kAspectMin, kAspectMax}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 4);
  }

  SECTION("area 0.0039") {
    FilterConfig filter{
        {0.004, 0.006}, {kSolidityMin, kSolidityMax}, {kAspectMin, kAspectMax}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 8);
  }

  SECTION("area 0.0170, 0.0678, 0.2713") {
    FilterConfig filter{
        {0.01, 1.0}, {kSolidityMin, kSolidityMax}, {kAspectMin, kAspectMax}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 7);
  }
}

TEST_CASE("solidity filter", "[ops]") {
  cv::Mat frame{cv::imread(kSolidityFilter)};
  cv::Mat mask;
  MaskFrame(frame, mask, kHsvLow, kHsvHigh);

  Contours contours;
  FindContours(mask, contours);
  REQUIRE(contours.size() == 8);

  // std::vector<cv::Point> hull;
  // cv::convexHull(contours[1], hull);
  // contours.push_back(hull);
  // MaskWrite(mask, contours);

  Contours filtered_contours;

  SECTION("no filters") {
    FilterConfig filter;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == contours.size());
  }

  SECTION("solidity 0.35") {
    FilterConfig filter{
        {kAreaMin, kAreaMax}, {0.35, 0.37}, {kAspectMin, kAspectMax}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 5);
  }

  SECTION("solidity 0.4899") {
    FilterConfig filter{
        {kAreaMin, kAreaMax}, {0.48, 0.49}, {kAspectMin, kAspectMax}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 1);
  }

  SECTION("solidity 1.0") {
    FilterConfig filter{{kAreaMin, kAreaMax},
                        {kSolidityMax - 1e-3, kSolidityMax},
                        {kAspectMin, kAspectMax}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 1);
  }
}

TEST_CASE("aspect filter", "[ops]") {
  cv::Mat frame{cv::imread(kAspectFilter)};
  cv::Mat mask;
  MaskFrame(frame, mask, kHsvLow, kHsvHigh);

  Contours contours;
  FindContours(mask, contours);
  REQUIRE(contours.size() == 5);

  Contours filtered_contours;

  SECTION("no filters") {
    FilterConfig filter;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == contours.size());
  }

  SECTION("aspect 0.1") {
    FilterConfig filter{
        {kAreaMin, kAreaMax}, {kSolidityMin, kSolidityMax}, {0.1, 0.1}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 1);
  }

  SECTION("aspect 0.5") {
    FilterConfig filter{
        {kAreaMin, kAreaMax}, {kSolidityMin, kSolidityMax}, {0.5, 0.5}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 1);
  }

  SECTION("aspect 1.0") {
    FilterConfig filter{
        {kAreaMin, kAreaMax}, {kSolidityMin, kSolidityMax}, {1.0, 1.0}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 1);
  }

  SECTION("aspect 2.0") {
    FilterConfig filter{
        {kAreaMin, kAreaMax}, {kSolidityMin, kSolidityMax}, {2.0, 2.0}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 1);
  }

  SECTION("aspect 10.0") {
    FilterConfig filter{
        {kAreaMin, kAreaMax}, {kSolidityMin, kSolidityMax}, {10.0, 210.0}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 1);
  }
}

TEST_CASE("contours sorted after filtering", "[ops]") {
  cv::Mat frame = cv::imread(kTargetMaster);
  REQUIRE(frame.size() == cv::Size(1280, 720));
  cv::Mat mask;
  Contours contours;

  PipelineConfig pc{kTargetMasterPipelineConfig};
  MaskFrame(frame, mask, pc.HsvLow(), pc.HsvHigh());
  FindContours(mask, contours);
  REQUIRE(contours.size() == kTargetMasterNumContours);

  struct compare {
    bool operator()(cv::InputArray a, cv::InputArray b) const {
      return cv::contourArea(a) > cv::contourArea(b);
    }
  } comparator;

  REQUIRE_FALSE(
      std::is_sorted(std::begin(contours), std::end(contours), comparator));

  FilterConfig filter;
  Contours filtered_contours;
  GeometricContoursFilter(filter, contours, filtered_contours);

  REQUIRE(std::is_sorted(filtered_contours.begin(), filtered_contours.end(),
                         comparator));

  // for (auto& c : contours) WARN("contour area = " << cv::contourArea(c));
  // for (auto& c : filtered_contours) WARN("filtered_contour area = " <<
  // cv::contourArea(c));
}