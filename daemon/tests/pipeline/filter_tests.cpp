#include "pipeline_tests.h"

using namespace deadeye;

TEST_CASE("pipeline filter tests", "[pipeline][filter]") {
  cv::Mat frame = cv::imread(kTargetMaster);

  AbstractPipeline pipeline{0};
  pipeline.Configure(kTargetMasterCaptureConfig);
  PipelineConfig pipeline_config{kTargetMasterPipelineConfig};
  pipeline_config.filter.frame_area = frame.cols * frame.rows;

  SECTION("all disabled") {
    REQUIRE_FALSE(pipeline_config.filter.IsAreaEnabled());
    REQUIRE_FALSE(pipeline_config.filter.IsSolidityEnabled());
    REQUIRE_FALSE(pipeline_config.filter.IsAspectEnabled());

    pipeline.Configure(pipeline_config);
    pipeline.ProcessFrame(frame);
    Contours contours = pipeline.GetFilteredContours();

    REQUIRE(contours.size() == kTargetMasterNumContours);
  }

  SECTION("all enabled - full range") {
    FilterConfig filter{{0.0, 0.999}, {0.01, 1.0}, {0.0, 19.999}};
    filter.frame_area = frame.cols * frame.rows;
    pipeline_config.filter = filter;

    REQUIRE(pipeline_config.filter.IsAreaEnabled());
    REQUIRE(pipeline_config.filter.IsSolidityEnabled());
    REQUIRE(pipeline_config.filter.IsAspectEnabled());

    pipeline.Configure(pipeline_config);
    pipeline.ProcessFrame(frame);
    Contours contours = pipeline.GetFilteredContours();

    REQUIRE(contours.size() == kTargetMasterNumContours);
  }

  SECTION("area > 5e-05") {
    FilterConfig filter{
        {5e-05, 1.0}, {kSolidityMin, kSolidityMax}, {kAspectMin, kAspectMax}};
    filter.frame_area = frame.cols * frame.rows;
    pipeline_config.filter = filter;

    REQUIRE(pipeline_config.filter.IsAreaEnabled());
    REQUIRE_FALSE(pipeline_config.filter.IsSolidityEnabled());
    REQUIRE_FALSE(pipeline_config.filter.IsAspectEnabled());

    pipeline.Configure(pipeline_config);
    pipeline.ProcessFrame(frame);
    Contours contours = pipeline.GetFilteredContours();

    REQUIRE(contours.size() == 3);
  }
}

TEST_CASE("resize", "[.]") {
  cv::Mat frame = cv::imread(kTargetMaster);
  cv::Mat output;
  cv::resize(frame, output, cv::Size(320, 180), 0, 0, cv::INTER_CUBIC);
  cv::imwrite("output.jpg", output);
}