#include "pipeline_tests.h"

using namespace deadeye;

static double e = 1e-6;

TEST_CASE("BM004: GeometricContoursFilter 1280x720",
          "[.][filter][benchmark][" DEADEYE_UNIT "]") {
  cv::Mat frame = cv::imread(kTargetMaster);
  PipelineConfig pc{kTargetMasterPipelineConfig};

  cv::Mat mask;
  MaskFrame(frame, mask, pc.HsvLow(), pc.HsvHigh());

  Contours contours;
  FindContours(mask, contours);
  REQUIRE(contours.size() == kTargetMasterNumContours);

  FilterConfig filter{{kAreaMin + e, kAreaMax},
                      {kSolidityMin + e, kSolidityMax},
                      {kAspectMin, kAspectMax - e}};
  filter.frame_area = frame.size().area();
  Contours filtered_contours;
  BENCHMARK("all enabled") {
    GeometricContoursFilter(filter, contours, filtered_contours);
  };
  REQUIRE(filtered_contours == contours);
}
