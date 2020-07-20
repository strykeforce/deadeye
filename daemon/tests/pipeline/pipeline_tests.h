#pragma once

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "../data/test_data.h"
#include "catch2/catch.hpp"
#include "pipeline/abstract_pipeline.h"
#include "pipeline/min_area_rect_pipeline.h"
#include "pipeline/pipeline_ops.h"
#include "pipeline/upright_rect_pipeline.h"