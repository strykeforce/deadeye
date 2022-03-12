#pragma once

#include <catch2/catch.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "../data/test_data.h"
#include "config.h"
#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "pipeline/abstract_pipeline.h"
#include "pipeline/min_area_rect_pipeline.h"
#include "pipeline/pipeline_ops.h"
#include "pipeline/upright_rect_pipeline.h"