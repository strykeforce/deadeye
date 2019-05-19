#pragma once

#define de_xstr(s) de_str(s)
#define de_str(s) #s

#include <spdlog/spdlog.h>
#include <tinyfsm.hpp>

#include "pipeline/base_pipeline.hpp"
#include "pipeline/default_pipeline.hpp"
#include "pipeline/pipeline.hpp"
#include "pipeline/pipeline_config.hpp"
#include "pipeline/test_pattern_pipeline.hpp"

#include "camera.hpp"
#include "controller.hpp"
#include "lights.hpp"

#include "fsm.hpp"
