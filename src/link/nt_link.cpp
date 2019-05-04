#include <spdlog/spdlog.h>
#include <cstdlib>
#include "nt_link.hpp"

using namespace deadeye;
namespace log = spdlog;

void NetworkTablesLink::init() {
  inst = nt::GetDefaultInstance();
  nt::AddLogger(
      inst,
      [](const nt::LogMessage& msg) {
        log::log(nt2spdlogLevel(msg), msg.message);
      },
      0, UINT_MAX);
  if (std::getenv("DEADEYE_NT_SERVER")) {
    log::info("Starting NetworkTables server");
    nt::StartServer(inst, "persistent.ini", "", NT_DEFAULT_PORT);
  } else {
    const char* address = "127.0.0.1";
    log::info("Starting NetworkTables client connecting to {}", address);
    nt::StartClient(inst, address, NT_DEFAULT_PORT);
  }
};

bool NetworkTablesLink::poll() { return false; }

log::level::level_enum NetworkTablesLink::nt2spdlogLevel(
    const nt::LogMessage& msg) {
  switch (msg.level) {
    case NT_LOG_CRITICAL:
      return log::level::critical;
    case NT_LOG_ERROR:
      return log::level::err;
    case NT_LOG_WARNING:
      return log::level::warn;
    case NT_LOG_INFO:
      return log::level::info;
    default:
      return log::level::debug;
  }
};
