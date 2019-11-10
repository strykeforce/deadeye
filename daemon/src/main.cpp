#include <memory>
#include <spdlog/spdlog.h>
#ifdef __linux__
#include <spdlog/sinks/systemd_sink.h>
#endif

#include "deadeye.h"

using namespace deadeye;

namespace {
void ConfigureLogging() {
#ifdef __linux__
  if (std::getenv("JOURNAL_STREAM")) { // started by systemd
    spdlog::set_default_logger(spdlog::systemd_logger_st("deadeyed"));
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%t] %v");
    spdlog::debug("JOURNAL_STREAM={}", std::getenv("JOURNAL_STREAM"));
  }
#endif

  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%H:%M:%S.%e] [%t] [%^%l%$]: %v");
}
} // namespace

int main(int argc, char **argv) {
  ConfigureLogging();

  try {
    DEADEYE_INIT()
    Controller::GetInstance().Run();
  } catch (std::exception const &e) {
    spdlog::critical("Controller::Run exception: {}", e.what());
    std::exit(EXIT_FAILURE);
  }
  std::exit(EXIT_SUCCESS);
}
