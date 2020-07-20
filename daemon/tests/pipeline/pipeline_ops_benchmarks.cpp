#include "pipeline_tests.h"

using namespace deadeye;

static double e = 1e-6;

TEST_CASE("pipeline ops separate", "[.][ops]") {
  cv::Mat frame = cv::imread(kTargetMaster);
  PipelineConfig pc{kTargetMasterPipelineConfig};

  cv::Mat mask;
  BENCHMARK("MaskFrame 1280x720") {
    MaskFrame(frame, mask, pc.HsvLow(), pc.HsvHigh());
  };

  Contours contours;
  BENCHMARK("FindContours 1280x720") { FindContours(mask, contours); };
  REQUIRE(contours.size() == kTargetMasterNumContours);

  Contours filtered_contours;

  FilterConfig filter{{kAreaMin + e, kAreaMax},
                      {kSolidityMin + e, kSolidityMax},
                      {kAspectMin, kAspectMax - e}};
  filter.frame_area = frame.size().area();
  BENCHMARK("GeometricContoursFilter 1280x720") {
    GeometricContoursFilter(filter, contours, filtered_contours);
  };
  REQUIRE(filtered_contours == contours);

  BENCHMARK("TargetData") {
    auto td = std::make_unique<TargetData>("Z0", 0, false);
    return td->Dump();
  };
}

TEST_CASE("pipeline ops together", "[.][ops]") {
  cv::Mat frame = cv::imread(kTargetMaster);
  PipelineConfig pc{kTargetMasterPipelineConfig};

  cv::Mat mask;
  Contours contours;
  Contours filtered_contours;
  FilterConfig filter{{kAreaMin + e, kAreaMax},
                      {kSolidityMin + e, kSolidityMax},
                      {kAspectMin, kAspectMax - e}};
  filter.frame_area = frame.size().area();

  BENCHMARK("1280x720") {
    MaskFrame(frame, mask, pc.HsvLow(), pc.HsvHigh());
    FindContours(mask, contours);
    GeometricContoursFilter(filter, contours, filtered_contours);
    auto td = std::make_unique<TargetData>("Z0", 0, false);
    return td->Dump();
  };
}