#include <hedley.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>
#include <spdlog/spdlog.h>
#include <atomic>
#include <csignal>
#include <cstdlib>
#include <sstream>
#include "camera.hpp"
#include "controller.hpp"
#include "events.hpp"
#include "fsm.hpp"
#include "lights.hpp"
#include "nt_constants.hpp"

namespace {
static char const* kNTServerAddress = "127.0.0.1";
static constexpr double kPollTimeout = 0.25;
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
  try {
    nt::Flush(inst_);
    nt::DestroyEntryListenerPoller(poller_);
  } catch (std::exception const& e) {
    spdlog::error("~Controller: {}", e.what());
  }
  spdlog::info("Deadeye Controller exiting.");
}

/**
 * Run listens for commands and config changes.
 */
int Controller::Run() {
  fsm::start();

  for (bool timed_out = false;;) {
    auto entries = nt::PollEntryListener(poller_, kPollTimeout, &timed_out);

    // check for signal or network tables error condition
    if (HEDLEY_UNLIKELY(quit.load())) {
      spdlog::debug("Controller received shutdown signal");
      fsm::dispatch(CameraOff());  // all off
      fsm::dispatch(LightsOff());  // all off
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
        //
        // Camera events
        //
        case hash(DE_CAMERA_CONTROL("0", DE_ENABLED)):
          if (entry.value->GetBoolean())
            Camera<0>::dispatch(CameraOn());
          else
            Camera<0>::dispatch(CameraOff());
          break;
        case hash(DE_CAMERA_CONTROL("1", DE_ENABLED)):
          if (entry.value->GetBoolean())
            Camera<1>::dispatch(CameraOn());
          else
            Camera<1>::dispatch(CameraOff());
          break;
        //
        // Lights events
        //
        case hash(DE_LIGHTS_CONTROL("0", DE_ON)):
          if (entry.value->GetBoolean()) Lights<0>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("1", DE_ON)):
          if (entry.value->GetBoolean()) Lights<1>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("0", DE_BLINK)):
          if (entry.value->GetBoolean()) Lights<0>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("1", DE_BLINK)):
          if (entry.value->GetBoolean()) Lights<1>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("0", DE_OFF)):
          if (entry.value->GetBoolean()) Lights<0>::dispatch(LightsOff());
          break;
        case hash(DE_LIGHTS_CONTROL("1", DE_OFF)):
          if (entry.value->GetBoolean()) Lights<1>::dispatch(LightsOff());
          break;
        case hash(DE_CAMERA_CONTROL("0", DE_ERROR)):
        case hash(DE_CAMERA_CONTROL("1", DE_ERROR)):
          // camera error state updates NT but we don't dispatch event
          break;
        default:
          spdlog::warn("Controller: {} event unrecognized in {}, line {}",
                       entry.name, __FILE__, __LINE__);
          break;
      }
    }
  }
  return EXIT_SUCCESS;
}

/**
 * SetCameraStatus updates network tables with current state of lights.
 */
void Controller::SetCameraStatus(int inum, bool enabled) {
  auto val = nt::Value::MakeBoolean(enabled);
  NT_Entry entry;
  switch (inum) {
    case 0:
      entry = nt::GetEntry(inst_, DE_CAMERA_CONTROL("0", DE_ENABLED));
      break;
    case 1:
      entry = nt::GetEntry(inst_, DE_CAMERA_CONTROL("1", DE_ENABLED));
      break;
    default:
      spdlog::error("Unrecognized Camera<{}> in {}, line {}", inum, __FILE__,
                    __LINE__);
      return;
  }
  nt::SetEntryValue(entry, val);
}

/**
 * SetCameraError updates network tables with camera error.
 */
void Controller::SetCameraError(int inum, bool error) {
  auto val = nt::Value::MakeBoolean(error);
  NT_Entry entry;
  switch (inum) {
    case 0:
      entry = nt::GetEntry(inst_, DE_CAMERA_CONTROL("0", DE_ERROR));
      break;
    case 1:
      entry = nt::GetEntry(inst_, DE_CAMERA_CONTROL("1", DE_ERROR));
      break;
    default:
      spdlog::error("Unrecognized Camera<{}> in {}, line {}", inum, __FILE__,
                    __LINE__);
      return;
  }
  nt::SetEntryValue(entry, val);
}

/**
 * SetLightsStatus updates network tables with current state of lights.
 */
void Controller::SetLightsStatus(int inum, char const* name, bool state) {
  std::stringstream path;
  path << DE_CONTROL_TABLE << DE_CAMERA << inum << DE_LIGHTS << "/" << name;

  auto entry = nt::GetEntry(inst_, path.str().c_str());
  nt::SetEntryValue(entry, nt::Value::MakeBoolean(state));
}

/**
 * StartNetworkTables starts network tables in client or server mode.
 */
void Controller::StartNetworkTables() {
  inst_ = nt::GetDefaultInstance();

  nt::AddLogger(
      inst_,
      [](const nt::LogMessage& msg) {
        spdlog::log(Nt2spdlogLevel(msg), msg.message);
      },
      0, UINT_MAX);

  if (std::getenv("DEADEYE_NT_SERVER")) {
    spdlog::info("Starting NetworkTables server");
    nt::StartServer(inst_, "persistent.ini", "", NT_DEFAULT_PORT);
  } else {
    spdlog::info("Starting NetworkTables client connecting to {}",
                 kNTServerAddress);
    nt::StartClient(inst_, kNTServerAddress, NT_DEFAULT_PORT);
  }
}

/**
 * StartPoller sets up polling for command and config events.
 */
void Controller::StartPoller() {
  poller_ = nt::CreateEntryListenerPoller(inst_);
  entry_listener_ = nt::AddPolledEntryListener(
      poller_, DE_CONTROL_TABLE, NT_NOTIFY_LOCAL | NT_NOTIFY_UPDATE);
}

static void SetCameraControlTableDefaults(std::shared_ptr<NetworkTable> table) {
  table->SetDefaultBoolean(DE_ENABLED, false);
  table->SetDefaultBoolean(DE_ERROR, false);
  spdlog::debug("Setting default values for {}", table->GetPath().str());
}

static void SetLightsControlTableDefaults(std::shared_ptr<NetworkTable> table) {
  table->SetDefaultBoolean(DE_ON, false);
  table->SetDefaultBoolean(DE_OFF, false);
  table->SetDefaultBoolean(DE_BLINK, false);
  spdlog::debug("Setting default values for {}", table->GetPath().str());
}
/**
 * SetNetworkTablesDefaults sets up default values in network tables.
 */
void Controller::SetNetworkTablesDefaults() {
  auto nti = nt::NetworkTableInstance(inst_);
  SetCameraControlTableDefaults(nti.GetTable(DE_CAMERA_CONTROL_TABLE("0")));
  SetCameraControlTableDefaults(nti.GetTable(DE_CAMERA_CONTROL_TABLE("1")));
  SetLightsControlTableDefaults(nti.GetTable(DE_LIGHTS_CONTROL_TABLE("0")));
  SetLightsControlTableDefaults(nti.GetTable(DE_LIGHTS_CONTROL_TABLE("1")));
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
}
