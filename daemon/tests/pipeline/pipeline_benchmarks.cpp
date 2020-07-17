#include "pipeline_tests.h"

using namespace deadeye;

TEMPLATE_TEST_CASE("benchmark pipelines", "[.][benchmark]", DefaultPipeline,
                   UprightRectPipeline, MinAreaRectPipeline) {
  cv::Mat frame = cv::imread(kTargetMaster);

  TestType pipeline{0};

  PipelineConfig pipeline_config{0,         {50, 90},       {0, 255},
                                 {70, 277}, FilterConfig(), LogConfig()};

  SECTION("all filters") {
    FilterConfig filter{{0.0, 0.999}, {0.001, 1.0}, {0.0, 19.999}};
    filter.frame_area = frame.cols * frame.rows;
    pipeline_config.filter = filter;
    pipeline.ConfigPipeline(pipeline_config);
    REQUIRE(pipeline_config.filter.area_enabled);
    REQUIRE(pipeline_config.filter.solidity_enabled);
    REQUIRE(pipeline_config.filter.aspect_enabled);

    BENCHMARK("ProcessFrame") { return pipeline.ProcessFrame(frame); };
    Contours contours = pipeline.GetContours();
    Contours filtered_contours = pipeline.GetFilteredContours();
    REQUIRE(contours == filtered_contours);
  }

  SECTION("no filters") {
    pipeline_config.filter = FilterConfig();
    pipeline_config.filter.frame_area = frame.cols * frame.rows;
    pipeline.ConfigPipeline(pipeline_config);
    REQUIRE_FALSE(pipeline_config.filter.area_enabled);
    REQUIRE_FALSE(pipeline_config.filter.solidity_enabled);
    REQUIRE_FALSE(pipeline_config.filter.aspect_enabled);

    BENCHMARK("ProcessFrame") { return pipeline.ProcessFrame(frame); };
    Contours contours = pipeline.GetContours();
    Contours filtered_contours = pipeline.GetFilteredContours();
    REQUIRE(contours == filtered_contours);
  }

  SECTION("area filter > 5e-05") {
    FilterConfig filter{
        {5e-05, 1.0}, {kSolidityMin, kSolidityMax}, {kAspectMin, kAspectMax}};
    filter.frame_area = frame.cols * frame.rows;
    pipeline_config.filter = filter;
    pipeline.ConfigPipeline(pipeline_config);
    REQUIRE(pipeline_config.filter.area_enabled);
    REQUIRE_FALSE(pipeline_config.filter.solidity_enabled);
    REQUIRE_FALSE(pipeline_config.filter.aspect_enabled);

    BENCHMARK("ProcessFrame") { return pipeline.ProcessFrame(frame); };
    Contours contours = pipeline.GetContours();
    Contours filtered_contours = pipeline.GetFilteredContours();
    REQUIRE(filtered_contours.size() == 3);
  }

#if 0
  StreamConfig stream_config{0};
  stream_config.view = StreamConfig::View::mask;
  stream_config.contour = StreamConfig::Contour::all;
  pipeline.ConfigStream(stream_config);

  TargetDataPtr target_data =
      std::make_unique<TargetData>(DEADEYE_UNIT + std::to_string(0), 0, false);

  cv::Mat preview;
  pipeline.ProcessStreamFrame(preview, target_data.get());

  cv::imwrite("preview.jpg", preview);
#endif
}