// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <memory>
#include <utility>

#include "config.h"
#include "controller.h"
#include "log.h"
#include "pipeline/latency_pipeline.h"
#include "pipeline/min_area_rect_pipeline.h"
#include "pipeline/null_pipeline.h"
#include "pipeline/target_list_pipeline.h"
#include "pipeline/upright_rect_pipeline.h"

#ifdef DEADEYE_CAMERA0_PIPELINE
#define DE_P0                                 \
  std::make_unique<DEADEYE_CAMERA0_PIPELINE>( \
      0, DE_STRINGIFY(DEADEYE_CAMERA0_PIPELINE))
#else
#define DE_P0 std::unique_ptr<Pipeline>(nullptr)
#endif

#ifdef DEADEYE_CAMERA1_PIPELINE
#define DE_P1                                 \
  std::make_unique<DEADEYE_CAMERA1_PIPELINE>( \
      1, DE_STRINGIFY(DEADEYE_CAMERA1_PIPELINE))
#else
#define DE_P1 std::unique_ptr<::deadeye::Pipeline>(nullptr)
#endif

#ifdef DEADEYE_CAMERA2_PIPELINE
#define DE_P2                                 \
  std::make_unique<DEADEYE_CAMERA2_PIPELINE>( \
      2, DE_STRINGIFY(DEADEYE_CAMERA2_PIPELINE))
#else
#define DE_P2 std::unique_ptr<::deadeye::Pipeline>(nullptr)
#endif

#ifdef DEADEYE_CAMERA3_PIPELINE
#define DE_P3                                 \
  std::make_unique<DEADEYE_CAMERA3_PIPELINE>( \
      3, DE_STRINGIFY(DEADEYE_CAMERA3_PIPELINE))
#else
#define DE_P3 std::unique_ptr<::deadeye::Pipeline>(nullptr)
#endif

#ifdef DEADEYE_CAMERA4_PIPELINE
#define DE_P4                                 \
  std::make_unique<DEADEYE_CAMERA4_PIPELINE>( \
      4, DE_STRINGIFY(DEADEYE_CAMERA4_PIPELINE))
#else
#define DE_P4 std::unique_ptr<::deadeye::Pipeline>(nullptr)
#endif

#define DEADEYE_MAIN()                                                     \
  int main(int argc, char** argv) {                                        \
    try {                                                                  \
      ::deadeye::log::Configure("deadeye");                                \
      ::deadeye::Pipelines pipelines({DE_P0, DE_P1, DE_P2, DE_P3, DE_P4}); \
      ::deadeye::Controller::Initialize(std::move(pipelines));             \
      ::deadeye::Controller::GetInstance().Run();                          \
    } catch (const std::exception& e) {                                    \
      spdlog::critical("DEADEYE_MAIN: {}", e.what());                      \
      std::exit(EXIT_FAILURE);                                             \
    }                                                                      \
    std::exit(EXIT_SUCCESS);                                               \
  }
