#include "log.h"

#ifdef __linux__
#include <spdlog/sinks/systemd_sink.h>
#endif

void deadeye::log::Configure(std::string name) {
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%H:%M:%S.%e] [%t] [%^%l%$]: %v");

#ifdef __linux__
  if (std::getenv("JOURNAL_STREAM")) {  // started by systemd
    spdlog::set_default_logger(spdlog::systemd_logger_st(name));
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%t] %v");
    spdlog::info("JOURNAL_STREAM={}", std::getenv("JOURNAL_STREAM"));
  }
#endif
}
