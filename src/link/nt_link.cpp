#include <networktables/NetworkTableInstance.h>
#include <spdlog/spdlog.h>
#include <chrono>
#include <cstdlib>
#include <thread>
#include "nt_link.hpp"

namespace {
static const char* kNTServerAddress = "127.0.0.1";
static const char* kControlTable = "/Deadeye/Control";
// static const char* kConfigTable = "/Deadeye/Config";
}  // namespace

using namespace deadeye;
namespace log = spdlog;

void NetworkTablesLink::Init() {
  StartNetworkTables();
  SetDefaults();
  StartPoller();
};

bool NetworkTablesLink::Poll() {
  auto entries = nt::PollEntryListener(poller);
  for (auto& entry : entries) log::debug("poll: name = {}", entry.name);
  return true;
}

log::level::level_enum NetworkTablesLink::Nt2spdlogLevel(
    const nt::LogMessage& msg) {
  using namespace log::level;

  switch (msg.level) {
    case NT_LOG_CRITICAL:
      return critical;
    case NT_LOG_ERROR:
      return err;
    case NT_LOG_WARNING:
      return warn;
    case NT_LOG_INFO:
      return info;
    default:
      return debug;
  }
};

void NetworkTablesLink::StartNetworkTables() {
  inst = nt::GetDefaultInstance();

  nt::AddLogger(
      inst,
      [](const nt::LogMessage& msg) {
        log::log(Nt2spdlogLevel(msg), msg.message);
      },
      0, UINT_MAX);

  if (std::getenv("DEADEYE_NT_SERVER")) {
    log::info("Starting NetworkTables server");
    nt::StartServer(inst, "persistent.ini", "", NT_DEFAULT_PORT);
  } else {
    log::info("Starting NetworkTables client connecting to {}",
              kNTServerAddress);
    nt::StartClient(inst, kNTServerAddress, NT_DEFAULT_PORT);
  }

  // FIXME: use connection listener
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

void NetworkTablesLink::SetDefaults() {
  auto nti = nt::NetworkTableInstance(inst);
  auto table = nti.GetTable(kControlTable);
  log::debug("Deadeye table path = {}", table->GetPath().str());
  table->SetDefaultBoolean("bromated", true);
  table->SetDefaultString("flavor", "Spicy!");
  table->SetDefaultNumber("errors", 3);
}

void NetworkTablesLink::StartPoller() {
  poller = nt::CreateEntryListenerPoller(inst);
  entry_listener = nt::AddPolledEntryListener(
      poller, kControlTable, NT_NOTIFY_IMMEDIATE | NT_NOTIFY_UPDATE);
}
