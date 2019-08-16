#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>
#include <atomic>
#include <csignal>
#include <tinyfsm.hpp>

#include "camera.hpp"
#include "controller.hpp"
#include "lights.hpp"
#include "pipeline.hpp"
#include "stream_config.hpp"

namespace {
// static char const* kNTServerAddress = "titan.lan.j3ff.io";
static char const* kNTServerAddress = "127.0.0.1";
static constexpr double kPollTimeout = 0.25;

std::atomic<bool> quit{false};

void signal_handler(int signal) { quit = true; }

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

#ifdef DEADEYE_CAMERA0_PIPELINE
  Camera<0>::SetPipeline(std::make_unique<DEADEYE_CAMERA0_PIPELINE>(0));
  spdlog::info("Camera<0> pipeline: {}",
               DE_STRINGIFY(DEADEYE_CAMERA0_PIPELINE));
#else
  spdlog::info("Camera<0> pipeline: NA");
#endif
#ifdef DEADEYE_CAMERA1_PIPELINE
  Camera<1>::SetPipeline(std::make_unique<DEADEYE_CAMERA1_PIPELINE>(1));
  spdlog::info("Camera<1> pipeline: {}",
               DE_STRINGIFY(DEADEYE_CAMERA1_PIPELINE));
#else
  spdlog::info("Camera<1> pipeline: NA");
#endif

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
void Controller::Run() {
#ifdef DEADEYE_CAMERA0_PIPELINE
  Camera<0>::start();
  Lights<0>::start();
#endif
#ifdef DEADEYE_CAMERA1_PIPELINE
  Camera<1>::start();
  Lights<1>::start();
#endif

  for (bool timed_out = false;;) {
    auto entries = nt::PollEntryListener(poller_, kPollTimeout, &timed_out);

    // check for signal or network tables error condition
    if (quit.load()) {
      spdlog::debug("Controller received shutdown signal");
      ShutDown();
      return;
    }
    if (!timed_out && entries.empty()) {
      spdlog::critical("PollEntryListener entries is empty in {}, line {}",
                       __FILE__, __LINE__);
      throw std::runtime_error("NetworkTables error");
    }

    // issue FSM events for camera errors
    // The On state receiving CameraOff event will cancel pipeline task, catch
    // exception and transition to Error state.
#ifdef DEADEYE_CAMERA0_PIPELINE
    if (Camera<0>::HasError()) Camera<0>::dispatch(CameraOff());
#endif
#ifdef DEADEYE_CAMERA1_PIPELINE
    if (Camera<1>::HasError()) Camera<1>::dispatch(CameraOff());
#endif

    // issue FSM events from network tables entry update notifications
    for (const auto& entry : entries) {
      switch (hash(entry.name.c_str())) {
#ifdef DEADEYE_CAMERA0_PIPELINE
        //
        // Camera 0 events
        //
        case hash(DE_CAMERA_CONTROL("0", DE_ON)):
          if (entry.value->GetBoolean()) Camera<0>::dispatch(CameraOn());
          break;
        case hash(DE_CAMERA_CONTROL("0", DE_OFF)):
          if (entry.value->GetBoolean()) Camera<0>::dispatch(CameraOff());
          break;
        case hash(DE_LIGHTS_CONTROL("0", DE_ON)):
          if (entry.value->GetBoolean()) Lights<0>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("0", DE_BLINK)):
          if (entry.value->GetBoolean()) Lights<0>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("0", DE_OFF)):
          if (entry.value->GetBoolean()) Lights<0>::dispatch(LightsOff());
          break;
        case hash(DE_CAMERA_CONFIG_ENTRY("0")): {
          CameraConfig event;
          event.config = PipelineConfig::New(entry.value);
          Camera<0>::dispatch(event);
          break;
        }
#endif
#ifdef DEADEYE_CAMERA1_PIPELINE
        //
        // Camera 1 events
        //
        case hash(DE_CAMERA_CONTROL("1", DE_ON)):
          if (entry.value->GetBoolean()) Camera<1>::dispatch(CameraOn());
          break;
        case hash(DE_CAMERA_CONTROL("1", DE_OFF)):
          if (entry.value->GetBoolean()) Camera<1>::dispatch(CameraOff());
          break;
        case hash(DE_LIGHTS_CONTROL("1", DE_ON)):
          if (entry.value->GetBoolean()) Lights<1>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("1", DE_BLINK)):
          if (entry.value->GetBoolean()) Lights<1>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("1", DE_OFF)):
          if (entry.value->GetBoolean()) Lights<1>::dispatch(LightsOff());
          break;
        case hash(DE_CAMERA_CONFIG_ENTRY("1")): {
          CameraConfig event;
          event.config = PipelineConfig::New(entry.value);
          Camera<1>::dispatch(event);
          break;
        }
#endif
        default:
          spdlog::warn("Controller: {} event unrecognized in {}, line {}",
                       entry.name, __FILE__, __LINE__);
          break;
      }
    }
  }
}

void Controller::ShutDown() {
  // fsm dispatches to all instances
#ifdef DEADEYE_CAMERA0_PIPELINE
  Camera<0>::dispatch(CameraOff());
  Lights<0>::dispatch(LightsOff());
#endif
#ifdef DEADEYE_CAMERA1_PIPELINE
  Camera<1>::dispatch(CameraOff());
  Lights<1>::dispatch(LightsOff());
#endif
}

/**
 * SetCameraError updates network tables with camera error.
 */
void Controller::SetCameraStatus(int inum, char const* name, bool state) {
  std::stringstream path;
  path << DE_CONTROL_TABLE << "/" << inum << "/" << name;

  auto entry = nt::GetEntry(inst_, path.str().c_str());
  nt::SetEntryValue(entry, nt::Value::MakeBoolean(state));
}

/**
 * SetLightsStatus updates network tables with current state of lights.
 */
void Controller::SetLightsStatus(int inum, char const* name, bool state) {
  std::stringstream path;
  path << DE_CONTROL_TABLE << "/" << inum << DE_LIGHTS << "/" << name;

  auto entry = nt::GetEntry(inst_, path.str().c_str());
  nt::SetEntryValue(entry, nt::Value::MakeBoolean(state));
}

/**
 * StartNetworkTables starts network tables in client or server mode.
 */
void Controller::StartNetworkTables() {
  inst_ = nt::GetDefaultInstance();

  nt::AddLogger(inst_,
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
  entry_listener_ =
      nt::AddPolledEntryListener(poller_, DE_CONTROL_TABLE, NT_NOTIFY_UPDATE);
  nt::AddPolledEntryListener(poller_, DE_CONFIG_TABLE, NT_NOTIFY_UPDATE);
}

namespace {
void SetCameraControlTableDefaults(std::shared_ptr<NetworkTable> table) {
  table->SetDefaultBoolean(DE_ON, false);
  table->SetDefaultBoolean(DE_OFF, false);
  table->SetDefaultBoolean(DE_ERROR, false);
  spdlog::debug("Setting default values for {}", table->GetPath().str());
}

void SetLightsControlTableDefaults(std::shared_ptr<NetworkTable> table) {
  table->SetDefaultBoolean(DE_ON, false);
  table->SetDefaultBoolean(DE_OFF, false);
  table->SetDefaultBoolean(DE_BLINK, false);
  spdlog::debug("Setting default values for {}", table->GetPath().str());
}

void SetCameraConfigEntryDefault(nt::NetworkTableEntry entry) {
  PipelineConfig pc{0, {0, 254}, {0, 254}, {0, 254}, 0.5};
  json j = pc;
  entry.SetDefaultString(j.dump());
  entry.SetPersistent();
}

void SetStreamConfigEntryDefault(nt::NetworkTableEntry entry, int inum) {
  std::stringstream url;
  url << "http://localhost:" << std::to_string(5800 + inum) << "/stream.mjpg";
  StreamConfig sc{0, url.str()};
  json j = sc;
  entry.SetDefaultString(j.dump());
  entry.SetPersistent();
}
}  // namespace

/**
 * SetNetworkTablesDefaults sets up default values in network tables.
 */
void Controller::SetNetworkTablesDefaults() {
  auto nti = nt::NetworkTableInstance(inst_);
#ifdef DEADEYE_CAMERA0_PIPELINE
  SetCameraControlTableDefaults(nti.GetTable(DE_CAMERA_CONTROL_TABLE("0")));
  SetLightsControlTableDefaults(nti.GetTable(DE_LIGHTS_CONTROL_TABLE("0")));
  SetCameraConfigEntryDefault(nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("0")));
  SetStreamConfigEntryDefault(nti.GetEntry(DE_STREAM_CONFIG_ENTRY("0")), 0);
#endif
#ifdef DEADEYE_CAMERA1_PIPELINE
  SetCameraControlTableDefaults(nti.GetTable(DE_CAMERA_CONTROL_TABLE("1")));
  SetLightsControlTableDefaults(nti.GetTable(DE_LIGHTS_CONTROL_TABLE("1")));
  SetCameraConfigEntryDefault(nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("1")));
  SetStreamConfigEntryDefault(nti.GetEntry(DE_STREAM_CONFIG_ENTRY("1")), 1);
#endif
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
