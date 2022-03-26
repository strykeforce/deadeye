// Copyright (c) 2022 Stryke Force FRC 2767

#include "client_logger.h"

#include <networktables/NetworkTableInstance.h>
#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>
#include <string>

#include "config.h"

using ::deadeye::ClientLogger;
using json = nlohmann::json;

namespace {
constexpr std::string_view level2string(ClientLogger::Level level) {
  switch (level) {
    case ClientLogger::Level::debug:
      return "debug";
    case ClientLogger::Level::info:
      return "info";
    case ClientLogger::Level::warn:
      return "warn";
    case ClientLogger::Level::error:
      return "error";
  }
}

constexpr spdlog::level::level_enum level2spdlog(ClientLogger::Level level) {
  switch (level) {
    case ClientLogger::Level::debug:
      return spdlog::level::debug;
    case ClientLogger::Level::info:
      return spdlog::level::info;
    case ClientLogger::Level::warn:
      return spdlog::level::warn;
    case ClientLogger::Level::error:
      return spdlog::level::err;
  }
}
}  // namespace

ClientLogger::ClientLogger(int inum, nt::NetworkTableInstance nti) {
  entry_ = nti.GetEntry(deadeye::LogEntryPath(inum));
  entry_.ClearPersistent();
}

ClientLogger::ClientLogger(int inum)
    : ClientLogger(inum, nt::NetworkTableInstance::GetDefault()) {}

void ClientLogger::Log(ClientLogger::Level level, std::string_view msg) {
  json j{{"level", level2string(level)}, {"message", msg}};
  entry_.SetString(j.dump());
  entry_.GetInstance().Flush();
  spdlog::log(level2spdlog(level), msg);
}
