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
    REQUIRE(contours.size() == 0);
  }

  SECTION("tuned hsv range") {
    PipelineConfig pc{kTargetMasterPipelineConfig};
    MaskFrame(frame, mask, pc.HsvLow(), pc.HsvHigh());
    FindContours(mask, contours);
    REQUIRE(contours.size() == kTargetMasterNumContours);
  }
}

void MaskWrite(cv::Mat const &mask, Contours const &contours) {
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
    REQUIRE(filtered_contours.size() == 15);
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
                        {kSolidityMax, kSolidityMax},
                        {kAspectMin, kAspectMax}};
    filter.frame_area = 1280 * 720;
    GeometricContoursFilter(filter, contours, filtered_contours);
    REQUIRE(filtered_contours.size() == 1);
  }
}