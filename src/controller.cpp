#include <networktables/NetworkTableInstance.h>
#include <spdlog/spdlog.h>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <thread>
#include "controller.hpp"

namespace {
static const char* kNTServerAddress = "127.0.0.1";
static const char* kControlTable = "/Deadeye/Control";
static constexpr double kPollTimeout = 0.5;
// static const char* kConfigTable = "/Deadeye/Config";

std::atomic<bool> quit{false};

void signal_handler(int signal) { quit.store(true); }

/**
 * From http://www.rioki.org/2016/03/31/cpp-switch-string.html
 */
constexpr unsigned int hash(const char* str, int h = 0) {
  return !str[h] ? 5381 : (hash(str, h + 1) * 33) ^ str[h];
}
}  // namespace

using namespace deadeye;
namespace log = spdlog;

/**
 * Constructor for Controller.
 */
Controller::Controller() {
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  StartNetworkTables();
  SetNetworkTablesDefaults();
  StartPoller();
}

/**
 * Destructor for Controller.
 */
Controller::~Controller() {
  nt::DestroyEntryListenerPoller(poller);
  log::info("Deadeye Controller exiting.");
}

/**
 * Run listens for commands and config changes.
 */
int Controller::Run() {
  while (true) {
    bool timed_out{false};
    auto entries = nt::PollEntryListener(poller, kPollTimeout, &timed_out);

    // check for signal or error condition
    if (quit.load()) break;
    if (!timed_out && entries.empty()) {
      log::critical("PollEntryListener error.");
      return EXIT_FAILURE;
    }

    for (auto& entry : entries) {
      switch (hash(entry.name.c_str())) {
        case hash("/Deadeye/Control/flavor"):
          log::debug("{} = {}", entry.name, entry.value->GetString().str());
          break;
        case hash("/Deadeye/Control/bromated"):
          log::debug("{} = {}", entry.name, entry.value->GetBoolean());
          break;
        case hash("/Deadeye/Control/errors"):
          log::debug("{} = {}", entry.name, entry.value->GetDouble());
          break;
        default:
          log::debug("{} unrecognized", entry.name);
      }
    }
  }
  return EXIT_SUCCESS;
}

/**
 * StartNetworkTables starts network tables in client or server mode.
 */
void Controller::StartNetworkTables() {
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

/**
 * StartPoller sets up polling for command and config events.
 */
void Controller::StartPoller() {
  poller = nt::CreateEntryListenerPoller(inst);
  entry_listener = nt::AddPolledEntryListener(
      poller, kControlTable, NT_NOTIFY_IMMEDIATE | NT_NOTIFY_UPDATE);
}

/**
 * SetNetworkTablesDefaults sets up default values in network tables.
 */
void Controller::SetNetworkTablesDefaults() {
  auto nti = nt::NetworkTableInstance(inst);
  auto table = nti.GetTable(kControlTable);
  log::debug("Deadeye table path = {}", table->GetPath().str());
  table->SetDefaultBoolean("bromated", true);
  table->SetDefaultString("flavor", "Spicy!");
  table->SetDefaultNumber("errors", 3);
}

/**
 * Nt2spdlogLevel converts logging levels.
 */
log::level::level_enum Controller::Nt2spdlogLevel(const nt::LogMessage& msg) {
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
