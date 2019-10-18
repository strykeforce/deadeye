#include <spdlog/spdlog.h>
#include <memory>

#include "config/deadeye_config.hpp"
#include "controller.hpp"
#include "pipeline/default_pipeline.hpp"
#include "pipeline/driver_pipeline.hpp"
#include "pipeline/test_pattern_pipeline.hpp"

#ifndef DEADEYE_CAMERA0_PIPELINE
#define DEADEYE_CAMERA0_PIPELINE NullPipeline
#endif
#ifndef DEADEYE_CAMERA1_PIPELINE
#define DEADEYE_CAMERA1_PIPELINE NullPipeline
#endif
#ifndef DEADEYE_CAMERA2_PIPELINE
#define DEADEYE_CAMERA2_PIPELINE NullPipeline
#endif
#ifndef DEADEYE_CAMERA3_PIPELINE
#define DEADEYE_CAMERA3_PIPELINE NullPipeline
#endif
#ifndef DEADEYE_CAMERA4_PIPELINE
#define DEADEYE_CAMERA4_PIPELINE NullPipeline
#endif

using namespace deadeye;

namespace {
void ConfigureLogging() {
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%H:%M:%S.%e] [%t] [%^%l%$]: %v");
}
}  // namespace

int main(int argc, char** argv) {
  ConfigureLogging();
  spdlog::info("Deadeye unit {} starting", DEADEYE_UNIT);

  try {
    Controller::Initialize({
        std::make_unique<DEADEYE_CAMERA0_PIPELINE>(0),
        std::make_unique<DEADEYE_CAMERA1_PIPELINE>(1),
        std::make_unique<DEADEYE_CAMERA2_PIPELINE>(2),
        std::make_unique<DEADEYE_CAMERA3_PIPELINE>(3),
        std::make_unique<DEADEYE_CAMERA4_PIPELINE>(4),
    });

    Controller::GetInstance().Run();
  } catch (std::exception const& e) {
    spdlog::critical("Controller::Run exception: {}", e.what());
    std::exit(EXIT_FAILURE);
  }
  std::exit(EXIT_SUCCESS);
}
