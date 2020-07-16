#pragma once

#include <string>

#include "config/capture_config.h"
#include "config/deadeye_config.h"
#include "config/filter_config.h"
#include "config/log_config.h"
#include "config/pipeline_config.h"

static constexpr int kTargetMasterNumContours = 17;
static std::string kTargetMaster{DEADEYE_TEST_DATA "target.jpg"};

static deadeye::CaptureConfig kTargetMasterCaptureConfig{
    deadeye::CaptureConfig::Type::test, 1280, 720, 1280, 720, 120, 0, 0.1};

static deadeye::PipelineConfig kTargetMasterPipelineConfig{
    0,
    {50, 90},
    {0, 255},
    {70, 277},
    deadeye::FilterConfig(),
    deadeye::LogConfig()};

static std::string kAreaFilter{DEADEYE_TEST_DATA "area_filter.jpg"};
