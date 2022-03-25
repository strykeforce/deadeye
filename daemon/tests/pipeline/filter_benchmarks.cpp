// Copyright (c) 2022 Stryke Force FRC 2767

#include "pipeline_tests.h"

static double e = 1e-6;

TEST_CASE("BM004: GeometricContoursFilter 1280x720",
          "[.][filter][benchmark][" DEADEYE_UNIT "]") {
  cv::Mat frame = cv::imread(kTargetMaster);
  deadeye::PipelineConfig pc{kTargetMasterPipelineConfig};

  cv::Mat mask;
  deadeye::MaskFrame(frame, mask, pc.HsvLow(), pc.HsvHigh());

  deadeye::Contours contours;
  deadeye::FindContours(mask, contours);
  REQUIRE(contours.size() == kTargetMasterNumContours);

  deadeye::FilterConfig filter{{deadeye::kAreaMin + e, deadeye::kAreaMax},
                      {deadeye::kSolidityMin + e, deadeye::kSolidityMax},
                      {deadeye::kAspectMin, deadeye::kAspectMax - e}};
  filter.frame_area = frame.size().area();
  deadeye::Contours filtered_contours;
  BENCHMARK("all enabled") {
    GeometricContoursFilter(filter, contours, filtered_contours);
  };
  REQUIRE(filtered_contours == contours);
}
