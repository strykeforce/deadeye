#pragma once

#include "config/deadeye_config.h"
#include "controller.h"
#include "log.h"
#include "pipeline/driver_pipeline.h"
#include "pipeline/min_area_rect_pipeline.h"
#include "pipeline/upright_rect_pipeline.h"

#ifdef DEADEYE_CAMERA0_PIPELINE
#define DE_P0 std::make_unique<DEADEYE_CAMERA0_PIPELINE>(0)
#else
#define DE_P0 std::unique_ptr<Pipeline>(nullptr)
#endif

#ifdef DEADEYE_CAMERA1_PIPELINE
#define DE_P1 std::make_unique<DEADEYE_CAMERA1_PIPELINE>(1)
#else
#define DE_P1 std::unique_ptr<::deadeye::Pipeline>(nullptr)
#endif

#ifdef DEADEYE_CAMERA2_PIPELINE
#define DE_P2 std::make_unique<DEADEYE_CAMERA2_PIPELINE>(2)
#else
#define DE_P2 std::unique_ptr<::deadeye::Pipeline>(nullptr)
#endif

#ifdef DEADEYE_CAMERA3_PIPELINE
#define DE_P3 std::make_unique<DEADEYE_CAMERA3_PIPELINE>(3)
#else
#define DE_P3 std::unique_ptr<::deadeye::Pipeline>(nullptr)
#endif

#ifdef DEADEYE_CAMERA4_PIPELINE
#define DE_P4 std::make_unique<DEADEYE_CAMERA4_PIPELINE>(4)
#else
#define DE_P4 std::unique_ptr<::deadeye::Pipeline>(nullptr)
#endif

#define DEADEYE_MAIN()                                                        \
  int main(int argc, char **argv) {                                           \
    try {                                                                     \
      ::deadeye::log::Configure("deadeye");                                   \
      ::deadeye::Controller::Initialize({DE_P0, DE_P1, DE_P2, DE_P3, DE_P4}); \
      ::deadeye::Controller::GetInstance().Run();                             \
    } catch (std::exception const &e) {                                       \
      spdlog::critical("DEADEYE_MAIN: {}", e.what());                         \
      std::exit(EXIT_FAILURE);                                                \
    }                                                                         \
    std::exit(EXIT_SUCCESS);                                                  \
  }
