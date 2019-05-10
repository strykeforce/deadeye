#include <hedley.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>
#include <spdlog/spdlog.h>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <thread>
#include "camera.hpp"
#include "controller.hpp"
#include "fsm.hpp"
#include "lights.hpp"
#include "nt_constants.hpp"

namespace {
std::string_view kNTServerAddress{"127.0.0.1"};
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
  spdlog::info("Deadeye Controller exiting.");
}

/**
 * Run listens for commands and config changes.
 */
int Controller::Run() {
  fsm::start();

  for (bool timed_out = false;;) {
    auto entries = nt::PollEntryListener(poller, kPollTimeout, &timed_out);

    // check for signal or network tables error condition
    if (HEDLEY_UNLIKELY(quit.load())) {
      fsm::dispatch(CameraOff());
      spdlog::debug("Controller recieved shutdown signal");
      return EXIT_SUCCESS;
    }
    if (HEDLEY_UNLIKELY(!timed_out && entries.empty())) {
      spdlog::critical("PollEntryListener entries is empty in {}, line {}",
                       __FILE__, __LINE__);
      return EXIT_FAILURE;
    }

    // issue FSM events from network tables entry update notifications
    for (const auto& entry : entries) {
      switch (hash(entry.name.c_str())) {
        case hash(DE_CAMERA0_CONTROL(DE_ENABLED)):
          if (entry.value->GetBoolean())
            fsm::dispatch(CameraOn());
          else
            fsm::dispatch(CameraOff());
          break;
        case hash(DE_CAMERA0_CONTROL(DE_LIGHTS)):
          if (entry.value->GetBoolean())
            fsm::dispatch(LightsOn());
          else
            fsm::dispatch(LightsOff());
          break;
        default:
          spdlog::debug("{} unrecognized", entry.name);
          break;
      }
    }
  }
  return EXIT_SUCCESS;
}

/**
 * EnableLights updates network tables with current state of lights.
 */
void Controller::EnableLights(bool enabled) {
  auto val = nt::Value::MakeBoolean(enabled);
  auto entry = nt::GetEntry(inst, DE_CAMERA0_CONTROL(DE_LIGHTS));
  nt::SetEntryValue(entry, val);
}

/**
 * StartNetworkTables starts network tables in client or server mode.
 */
void Controller::StartNetworkTables() {
  inst = nt::GetDefaultInstance();

  nt::AddLogger(
      inst,
      [](const nt::LogMessage& msg) {
        spdlog::log(Nt2spdlogLevel(msg), msg.message);
      },
      0, UINT_MAX);

  if (std::getenv("DEADEYE_NT_SERVER")) {
    spdlog::info("Starting NetworkTables server");
    nt::StartServer(inst, "persistent.ini", "", NT_DEFAULT_PORT);
  } else {
    spdlog::info("Starting NetworkTables client connecting to {}",
                 kNTServerAddress);
    nt::StartClient(inst, kNTServerAddress.data(), NT_DEFAULT_PORT);
  }
}

/**
 * StartPoller sets up polling for command and config events.
 */
void Controller::StartPoller() {
  poller = nt::CreateEntryListenerPoller(inst);
  entry_listener = nt::AddPolledEntryListener(
      poller, DE_CONTROL_TABLE, NT_NOTIFY_IMMEDIATE | NT_NOTIFY_UPDATE);
}

/**
 * SetNetworkTablesDefaults sets up default values in network tables.
 */
void Controller::SetNetworkTablesDefaults() {
  auto nti = nt::NetworkTableInstance(inst);
  auto table = nti.GetTable(DE_CAMERA0_CONTROL_TABLE);
  spdlog::debug("Setting default values for {}", table->GetPath().str());
  table->SetDefaultBoolean(DE_ENABLED, false);
  table->SetDefaultBoolean(DE_LIGHTS, false);
}

/**
 * Nt2spdlogLevel converts logging levels.
 */
spdlog::level::level_enum Controller::Nt2spdlogLevel(
    const nt::LogMessage& msg) {
  using namespace spdlog::level;

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
