#include "client_logger.h"

#include <networktables/NetworkTableInstance.h>

#include <nlohmann/json.hpp>

#include "config.h"

using namespace deadeye;
using json = nlohmann::json;

namespace {
std::string_view level2string(ClientLogger::Level level) {
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
}