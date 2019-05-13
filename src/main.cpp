#include <spdlog/spdlog.h>
#include "controller.hpp"

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

  int rv;
  try {
    rv = Controller::GetInstance().Run();
  } catch (std::exception const& e) {
    spdlog::critical("Controller::Run: {}", e.what());
  }
  return rv;
}
