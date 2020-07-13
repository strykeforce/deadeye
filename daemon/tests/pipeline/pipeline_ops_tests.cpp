#include <opencv2/imgcodecs.hpp>

#include "catch2/catch.hpp"
#include "pipeline/pipeline_ops.h"

using namespace deadeye;

TEST_CASE("Benchmark Frame Ops", "[.][benchmark]") {
  cv::Mat frame = cv::imread(DEADEYE_TEST_DATA "target_01.jpg");
  REQUIRE(frame.rows == 303);
  REQUIRE(frame.cols == 404);

  cv::Mat mask;
  cv::Scalar low{0, 0, 0};
  cv::Scalar high{255, 255, 255};
  Contours contours;

  BENCHMARK("MaskFrame") { MaskFrame(frame, mask, low, high); };
  BENCHMARK("FindContours") { FindContours(mask, contours); };
}