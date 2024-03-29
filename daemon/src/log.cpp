// Copyright (c) 2022 Stryke Force FRC 2767

#include "log.h"

#include <spdlog/cfg/env.h>

#ifdef __linux__
#include <fmt/core.h>
#include <spdlog/sinks/systemd_sink.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#endif

void deadeye::log::Configure(const std::string& name) {
  spdlog::cfg::load_env_levels();  // take from SPDLOG_LEVEL

#ifdef __linux__
  const char* journal = std::getenv("JOURNAL_STREAM");
  if (journal != nullptr) {  // started by systemd
    struct stat es{};
    if (fstat(STDERR_FILENO, &es) != 0) {
      spdlog::error("Failed to stat stderr: {}", std::strerror(errno));
      return;
    }

    if (fmt::format("{}:{}", es.st_dev, es.st_ino) == std::string{journal}) {
      spdlog::set_default_logger(spdlog::systemd_logger_st(name));
      spdlog::set_pattern("[%t] %v");
      spdlog::info("Configuring logging for systemd");
      return;
    }
  }
#endif
  spdlog::set_pattern("[%H:%M:%S.%e] [%t] [%^%l%$]: %v");
}
