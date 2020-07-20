#include "pipeline_tests.h"

using namespace deadeye;

namespace {
FilterConfig GetFilterAll(cv::Size size) {
  double e = 1e-6;
  FilterConfig f{{kAreaMin + e, kAreaMax},
                 {kSolidityMin + e, kSolidityMax},
                 {kAspectMin, kAspectMax - e}};
  f.frame_area = size.area();
  REQUIRE(f.enabled);
  REQUIRE(f.area_enabled);
  REQUIRE(f.solidity_enabled);
  REQUIRE(f.aspect_enabled);
  return f;
}

FilterConfig GetFilterNone(cv::Size size) {
  FilterConfig f;
  f.frame_area = size.area();
  REQUIRE_FALSE(f.enabled);
  REQUIRE_FALSE(f.area_enabled);
  REQUIRE_FALSE(f.solidity_enabled);
  REQUIRE_FALSE(f.aspect_enabled);
  return f;
}
}  // namespace

TEMPLATE_TEST_CASE("benchmark pipelines", "[.][benchmark]", AbstractPipeline,
                   UprightRectPipeline, MinAreaRectPipeline) {
  cv::Mat frame = cv::imread(kTargetMaster);

  TestType pipeline{0};

  PipelineConfig pipeline_config{0,         {50, 90},       {0, 255},
                                 {70, 277}, FilterConfig(), LogConfig()};

  FilterConfig no_filters;

  SECTION("1280x720") {
    // all filters
    pipeline_config.filter = GetFilterAll(frame.size());
    pipeline.Configure(pipeline_config);

    BENCHMARK("all filters") {
      auto td = pipeline.ProcessFrame(frame);
      return td->Dump();
    };
    Contours contours = pipeline.GetContours();
    Contours filtered_contours = pipeline.GetFilteredContours();
    REQUIRE(contours.size() == filtered_contours.size());

    // no filters
    pipeline_config.filter = GetFilterNone(frame.size());
    pipeline.Configure(pipeline_config);

    BENCHMARK("no filters") {
      auto td = pipeline.ProcessFrame(frame);
      return td->Dump();
    };
    contours = pipeline.GetContours();
    filtered_contours = pipeline.GetFilteredContours();
    REQUIRE(contours.size() == filtered_contours.size());
  }

  SECTION("960x540") {
    cv::resize(frame, frame, cv::Size(960, 540));

    // all filters
    pipeline_config.filter = GetFilterAll(frame.size());
    pipeline.Configure(pipeline_config);

    BENCHMARK("all filters") {
      auto td = pipeline.ProcessFrame(frame);
      return td->Dump();
    };

    // no filters
    pipeline_config.filter = GetFilterNone(frame.size());
    pipeline.Configure(pipeline_config);

    BENCHMARK("no filters") {
      auto td = pipeline.ProcessFrame(frame);
      return td->Dump();
    };
  }

  SECTION("640x360") {
    cv::resize(frame, frame, cv::Size(640, 360));

    // all filters
    pipeline_config.filter = GetFilterAll(frame.size());
    pipeline.Configure(pipeline_config);

    BENCHMARK("all filters") {
      auto td = pipeline.ProcessFrame(frame);
      return td->Dump();
    };

    // no filters
    pipeline_config.filter = GetFilterNone(frame.size());
    pipeline.Configure(pipeline_config);

    BENCHMARK("no filters") {
      auto td = pipeline.ProcessFrame(frame);
      return td->Dump();
    };
  }

  SECTION("320x180") {
    cv::resize(frame, frame, cv::Size(320, 180));

    // all filters
    pipeline_config.filter = GetFilterAll(frame.size());
    pipeline.Configure(pipeline_config);

    BENCHMARK("all filters") {
      auto td = pipeline.ProcessFrame(frame);
      return td->Dump();
    };

    // no filteres
    pipeline_config.filter = GetFilterNone(frame.size());
    pipeline.Configure(pipeline_config);

    BENCHMARK("no filters") {
      auto td = pipeline.ProcessFrame(frame);
      return td->Dump();
    };
  }

#if 0
  StreamConfig stream_config{0};
  stream_config.view = StreamConfig::View::mask;
  stream_config.contour = StreamConfig::Contour::all;
  pipeline.Configure(stream_config);

  TargetDataPtr target_data =
      std::make_unique<TargetData>(DEADEYE_UNIT + std::to_string(0), 0, false);

  cv::Mat preview;
  pipeline.ProcessStreamFrame(preview, target_data.get());

  cv::imwrite("preview.jpg", preview);
#endif
}