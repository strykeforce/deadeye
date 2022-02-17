#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma once

#include <opencv2/core/types.hpp>
#include <string>

#include "config/capture_config.h"
#include "config/deadeye_config.h"
#include "config/filter_config.h"
#include "config/log_config.h"
#include "config/pipeline_config.h"

static constexpr int kTargetMasterNumContours = 17;
static std::string kTargetMaster{DEADEYE_TEST_DATA "target.jpg"};

static deadeye::CaptureConfig kTargetMasterCaptureConfig{
    deadeye::CaptureType::test, 1280, 720, 120, "{}"};

static deadeye::PipelineConfig kTargetMasterPipelineConfig{
    0,
    {50, 90},
    {0, 255},
    {70, 277},
    deadeye::FilterConfig(),
    deadeye::LogConfig()};

// test images below will mask with these HSV values
static cv::Scalar kHsvLow{32, 130, 200};
static cv::Scalar kHsvHigh{110, 255, 255};

static std::string kAreaFilter{DEADEYE_TEST_DATA "area_filter.jpg"};
static std::string kSolidityFilter{DEADEYE_TEST_DATA "solidity_filter.jpg"};
static std::string kAspectFilter{DEADEYE_TEST_DATA "aspect_filter.jpg"};

#pragma clang diagnostic pop