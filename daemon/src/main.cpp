#include <spdlog/spdlog.h>
#include <memory>

#include "controller.hpp"
#include "defs.hpp"
#include "pipeline.hpp"

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
        std::make_unique<TestPatternPipeline>(0),
        std::make_unique<TestPatternPipeline>(1),
    });

    Controller::GetInstance().Run();
  } catch (std::exception const& e) {
    spdlog::critical("Controller::Run exception: {}", e.what());
    std::exit(EXIT_FAILURE);
  }
  std::exit(EXIT_SUCCESS);
}
