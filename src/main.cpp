#include <spdlog/spdlog.h>

#include "camera.hpp"
#include "controller.hpp"
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
  spdlog::info("Deadeye starting");

  try {
    Controller::GetInstance().Run();
  } catch (std::exception const& e) {
    spdlog::critical("Controller::Run exception: {}", e.what());
    try {
      // stop any pipeline tasks left running
      Controller::GetInstance().ShutDown();
    } catch (...) {
    }
    std::exit(EXIT_FAILURE);
  }
  std::exit(EXIT_SUCCESS);
}
