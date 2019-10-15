#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>
#include <ntcore_cpp.h>
#include <spdlog/spdlog.h>
#include <atomic>
#include <csignal>
#include <future>
#include <tinyfsm.hpp>

#include "config/link_config.hpp"
#include "controller.hpp"
#include "defs.hpp"
#include "fsm/camera.hpp"
#include "fsm/lights.hpp"
#include "pipeline.hpp"

// forward declaration
static spdlog::level::level_enum Nt2spdlogLevel(const nt::LogMessage& msg);

using namespace deadeye;

namespace {
static constexpr double kPollTimeout = 0.25;

std::atomic<bool> quit{false};

void signal_handler(int signal) { quit = true; }

/**
 * From http://www.rioki.org/2016/03/31/cpp-switch-string.html
 */
constexpr unsigned int hash(const char* str, int h = 0) {
  return !str[h] ? 5381 : (hash(str, h + 1) * 33) ^ str[h];
}

std::array<std::unique_ptr<Pipeline>, 5> EMPTY = {
    std::unique_ptr<Pipeline>{nullptr}, std::unique_ptr<Pipeline>{nullptr},
    std::unique_ptr<Pipeline>{nullptr}, std::unique_ptr<Pipeline>{nullptr},
    std::unique_ptr<Pipeline>{nullptr}};
}  // namespace

/**
 * Constructor for Controller.
 */
Controller::Controller(
    std::array<std::unique_ptr<Pipeline>, 5>* const pipelines) {
  if (pipelines == nullptr) {
    spdlog::critical("Controller not initialized");
    throw std::runtime_error{"critical error"};
  }

  for (size_t i = 0; i < pipelines->size(); i++) {
    if (!(*pipelines)[i]) {
      (*pipelines)[i].reset(new NullPipeline(i));
      has_active_pipeline_[i] = false;
    } else {
      has_active_pipeline_[i] = true;
    }
    if ((*pipelines)[i]->GetInum() != static_cast<int>(i)) {
      spdlog::critical("{} does not match position in initialization array: {}",
                       *(*pipelines)[i], i);
      throw std::runtime_error("critical error");
    }
    spdlog::debug("pipeline[{}] = {}", i, *(*pipelines)[i]);
  }

  StartNetworkTables();
  InitializeNetworkTableEntries();

  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  Camera<0>::SetPipeline(std::move((*pipelines)[0]));
  Camera<1>::SetPipeline(std::move((*pipelines)[1]));
  Camera<2>::SetPipeline(std::move((*pipelines)[2]));
  Camera<3>::SetPipeline(std::move((*pipelines)[3]));
  Camera<4>::SetPipeline(std::move((*pipelines)[4]));

  InitializeCameraConfig();
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
  Camera<0>::start();
  Lights<0>::start();
  Camera<1>::start();
  Lights<1>::start();
  Camera<2>::start();
  Lights<2>::start();
  Camera<3>::start();
  Lights<3>::start();
  Camera<4>::start();
  Lights<4>::start();

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
    if (Camera<0>::HasError()) Camera<0>::dispatch(CameraOff());
    if (Camera<1>::HasError()) Camera<1>::dispatch(CameraOff());
    if (Camera<2>::HasError()) Camera<2>::dispatch(CameraOff());
    if (Camera<3>::HasError()) Camera<3>::dispatch(CameraOff());
    if (Camera<4>::HasError()) Camera<4>::dispatch(CameraOff());

    // issue FSM events from network tables entry update notifications
    for (const auto& entry : entries) {
      switch (hash(entry.name.c_str())) {
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
          spdlog::debug("Controller: new Pipeline<0> config event");
          ConfigCamera event;
          event.config =
              new PipelineConfig(entry.value);  // ownership passed, deleted in
                                                // BasePipeline::UpdateConfig
          Camera<0>::dispatch(event);
          break;
        }
        case hash(DE_STREAM_CONFIG_ENTRY("0")): {
          ConfigStream event;
          event.config =
              new StreamConfig(entry.value);  // ownership passed, deleted in
                                              // BasePipeline::UpdateStream
          Camera<0>::dispatch(event);
          break;
        }
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
          ConfigCamera event;
          event.config =
              new PipelineConfig(entry.value);  // ownership passed, deleted in
                                                // BasePipeline::UpdateConfig
          Camera<1>::dispatch(event);
          break;
        }
        case hash(DE_STREAM_CONFIG_ENTRY("1")): {
          ConfigStream event;
          event.config =
              new StreamConfig(entry.value);  // ownership passed, deleted in
                                              // BasePipeline::UpdateStream
          Camera<1>::dispatch(event);
          break;
        }

        //
        // Camera 2 events
        //
        case hash(DE_CAMERA_CONTROL("2", DE_ON)):
          if (entry.value->GetBoolean()) Camera<2>::dispatch(CameraOn());
          break;
        case hash(DE_CAMERA_CONTROL("2", DE_OFF)):
          if (entry.value->GetBoolean()) Camera<2>::dispatch(CameraOff());
          break;
        case hash(DE_LIGHTS_CONTROL("2", DE_ON)):
          if (entry.value->GetBoolean()) Lights<2>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("2", DE_BLINK)):
          if (entry.value->GetBoolean()) Lights<2>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("2", DE_OFF)):
          if (entry.value->GetBoolean()) Lights<2>::dispatch(LightsOff());
          break;
        case hash(DE_CAMERA_CONFIG_ENTRY("2")): {
          ConfigCamera event;
          event.config =
              new PipelineConfig(entry.value);  // ownership passed, deleted in
                                                // BasePipeline::UpdateConfig
          Camera<2>::dispatch(event);
          break;
        }
        case hash(DE_STREAM_CONFIG_ENTRY("2")): {
          ConfigStream event;
          event.config =
              new StreamConfig(entry.value);  // ownership passed, deleted in
                                              // BasePipeline::UpdateStream
          Camera<2>::dispatch(event);
          break;
        }

        //
        // Camera 3 events
        //
        case hash(DE_CAMERA_CONTROL("3", DE_ON)):
          if (entry.value->GetBoolean()) Camera<3>::dispatch(CameraOn());
          break;
        case hash(DE_CAMERA_CONTROL("3", DE_OFF)):
          if (entry.value->GetBoolean()) Camera<3>::dispatch(CameraOff());
          break;
        case hash(DE_LIGHTS_CONTROL("3", DE_ON)):
          if (entry.value->GetBoolean()) Lights<3>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("3", DE_BLINK)):
          if (entry.value->GetBoolean()) Lights<3>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("3", DE_OFF)):
          if (entry.value->GetBoolean()) Lights<3>::dispatch(LightsOff());
          break;
        case hash(DE_CAMERA_CONFIG_ENTRY("3")): {
          ConfigCamera event;
          event.config =
              new PipelineConfig(entry.value);  // ownership passed, deleted in
                                                // BasePipeline::UpdateConfig
          Camera<3>::dispatch(event);
          break;
        }
        case hash(DE_STREAM_CONFIG_ENTRY("3")): {
          ConfigStream event;
          event.config =
              new StreamConfig(entry.value);  // ownership passed, deleted in
                                              // BasePipeline::UpdateStream
          Camera<3>::dispatch(event);
          break;
        }

        //
        // Camera 4 events
        //
        case hash(DE_CAMERA_CONTROL("4", DE_ON)):
          if (entry.value->GetBoolean()) Camera<4>::dispatch(CameraOn());
          break;
        case hash(DE_CAMERA_CONTROL("4", DE_OFF)):
          if (entry.value->GetBoolean()) Camera<4>::dispatch(CameraOff());
          break;
        case hash(DE_LIGHTS_CONTROL("4", DE_ON)):
          if (entry.value->GetBoolean()) Lights<4>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("4", DE_BLINK)):
          if (entry.value->GetBoolean()) Lights<4>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("4", DE_OFF)):
          if (entry.value->GetBoolean()) Lights<4>::dispatch(LightsOff());
          break;
        case hash(DE_CAMERA_CONFIG_ENTRY("4")): {
          ConfigCamera event;
          event.config =
              new PipelineConfig(entry.value);  // ownership passed, deleted in
                                                // BasePipeline::UpdateConfig
          Camera<4>::dispatch(event);
          break;
        }
        case hash(DE_STREAM_CONFIG_ENTRY("4")): {
          ConfigStream event;
          event.config =
              new StreamConfig(entry.value);  // ownership passed, deleted in
                                              // BasePipeline::UpdateStream
          Camera<4>::dispatch(event);
          break;
        }

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
  Camera<0>::dispatch(CameraOff());
  Lights<0>::dispatch(LightsOff());
  Camera<1>::dispatch(CameraOff());
  Lights<1>::dispatch(LightsOff());
  Camera<2>::dispatch(CameraOff());
  Lights<2>::dispatch(LightsOff());
  Camera<3>::dispatch(CameraOff());
  Lights<3>::dispatch(LightsOff());
  Camera<4>::dispatch(CameraOff());
  Lights<4>::dispatch(LightsOff());
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

  std::promise<void> barrier;
  std::future<void> barrier_future = barrier.get_future();

  auto conn_listener = nt::AddConnectionListener(
      inst_, [&](auto event) mutable { barrier.set_value(); }, true);

  if (std::getenv("DEADEYE_NT_SERVER")) {
    spdlog::info("Starting NetworkTables server");
    nt::StartServer(inst_, "persistent.ini", "", NT_DEFAULT_PORT);
  } else {
    spdlog::info("Starting NetworkTables client connecting to {}", NT_SERVER);
    nt::StartClient(inst_, NT_SERVER, NT_DEFAULT_PORT);
  }

  spdlog::debug("Waiting for connection...");
  barrier_future.wait();
  spdlog::debug("...connected");
  nt::RemoveConnectionListener(conn_listener);
}

/**
 * StartPoller sets up polling for command and config events.
 */
void Controller::StartPoller() {
  poller_ = nt::CreateEntryListenerPoller(inst_);
  entry_listener_ =
      nt::AddPolledEntryListener(poller_, DE_CONTROL_TABLE, NT_NOTIFY_UPDATE);
}

//
// Initialize NetworkTables defaults
//
namespace {
void SetCameraControlTableEntries(std::shared_ptr<NetworkTable> table) {
  table->PutBoolean(DE_ON, false);
  table->PutBoolean(DE_OFF, true);
  table->PutBoolean(DE_ERROR, false);
}

void SetLightsControlTableEntries(std::shared_ptr<NetworkTable> table) {
  table->PutBoolean(DE_ON, false);
  table->PutBoolean(DE_OFF, true);
  table->PutBoolean(DE_BLINK, false);
}

void SetCameraConfigEntryDefault(nt::NetworkTableEntry entry) {
  PipelineConfig pc{0,        {0, 254},
                    {0, 254}, {0, 254},
                    0.5,      GStreamerConfig{1280, 720, 320, 180, 60, 0}};
  json j = pc;
  entry.SetDefaultString(j.dump());
  entry.SetPersistent();
}

void SetStreamConfigEntry(nt::NetworkTableEntry entry, int inum) {
  StreamConfig sc{inum};
  json j = sc;
  entry.SetString(j.dump());
}

void SetLinkConfigEntry(nt::NetworkTableEntry entry) {
  LinkConfig lc{CLIENT_ADDRESS, CLIENT_PORT, true};
  json j = lc;
  entry.SetDefaultString(j.dump());
  entry.SetPersistent();
}

}  // namespace

/**
 * InitializeNetworkTableEntries sets up default values in network tables.
 */
void Controller::InitializeNetworkTableEntries() {
  auto nti = nt::NetworkTableInstance(inst_);
  SetLinkConfigEntry(nti.GetEntry(DE_LINK_CONFIG_ENTRY));
  if (has_active_pipeline_[0]) {
    SetCameraControlTableEntries(nti.GetTable(DE_CAMERA_CONTROL_TABLE("0")));
    SetLightsControlTableEntries(nti.GetTable(DE_LIGHTS_CONTROL_TABLE("0")));
    SetCameraConfigEntryDefault(nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("0")));
    SetStreamConfigEntry(nti.GetEntry(DE_STREAM_CONFIG_ENTRY("0")), 0);
  }

  if (has_active_pipeline_[1]) {
    SetCameraControlTableEntries(nti.GetTable(DE_CAMERA_CONTROL_TABLE("1")));
    SetLightsControlTableEntries(nti.GetTable(DE_LIGHTS_CONTROL_TABLE("1")));
    SetCameraConfigEntryDefault(nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("1")));
    SetStreamConfigEntry(nti.GetEntry(DE_STREAM_CONFIG_ENTRY("1")), 1);
  }

  if (has_active_pipeline_[2]) {
    SetCameraControlTableEntries(nti.GetTable(DE_CAMERA_CONTROL_TABLE("2")));
    SetLightsControlTableEntries(nti.GetTable(DE_LIGHTS_CONTROL_TABLE("2")));
    SetCameraConfigEntryDefault(nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("2")));
    SetStreamConfigEntry(nti.GetEntry(DE_STREAM_CONFIG_ENTRY("2")), 2);
  }

  if (has_active_pipeline_[3]) {
    SetCameraControlTableEntries(nti.GetTable(DE_CAMERA_CONTROL_TABLE("3")));
    SetLightsControlTableEntries(nti.GetTable(DE_LIGHTS_CONTROL_TABLE("3")));
    SetCameraConfigEntryDefault(nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("3")));
    SetStreamConfigEntry(nti.GetEntry(DE_STREAM_CONFIG_ENTRY("3")), 3);
  }

  if (has_active_pipeline_[4]) {
    SetCameraControlTableEntries(nti.GetTable(DE_CAMERA_CONTROL_TABLE("4")));
    SetLightsControlTableEntries(nti.GetTable(DE_LIGHTS_CONTROL_TABLE("4")));
    SetCameraConfigEntryDefault(nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("4")));
    SetStreamConfigEntry(nti.GetEntry(DE_STREAM_CONFIG_ENTRY("4")), 4);
  }
}

/**
 * InitializeCameraConfig loads config into active pipelines.
 */
void Controller::InitializeCameraConfig() {
  auto nti = nt::NetworkTableInstance(inst_);
  std::shared_ptr<nt::Value> value;

  if (has_active_pipeline_[0]) {
    value = nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("0")).GetValue();
    assert(value);
    Camera<0>::SetConfig(new PipelineConfig(value));  // ownership passed

    value = nti.GetEntry(DE_STREAM_CONFIG_ENTRY("0")).GetValue();
    assert(value);
    Camera<0>::SetStream(new StreamConfig(value));  // ownership passed
  }

  if (has_active_pipeline_[1]) {
    value = nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("1")).GetValue();
    assert(value);
    Camera<1>::SetConfig(new PipelineConfig(value));  // ownership passed

    value = nti.GetEntry(DE_STREAM_CONFIG_ENTRY("1")).GetValue();
    assert(value);
    Camera<1>::SetStream(new StreamConfig(value));  // ownership passed
  }

  if (has_active_pipeline_[2]) {
    value = nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("2")).GetValue();
    assert(value);
    Camera<2>::SetConfig(new PipelineConfig(value));  // ownership passed

    value = nti.GetEntry(DE_STREAM_CONFIG_ENTRY("2")).GetValue();
    assert(value);
    Camera<2>::SetStream(new StreamConfig(value));  // ownership passed
  }

  if (has_active_pipeline_[3]) {
    value = nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("3")).GetValue();
    assert(value);
    Camera<3>::SetConfig(new PipelineConfig(value));  // ownership passed

    value = nti.GetEntry(DE_STREAM_CONFIG_ENTRY("3")).GetValue();
    assert(value);
    Camera<3>::SetStream(new StreamConfig(value));  // ownership passed
  }

  if (has_active_pipeline_[4]) {
    value = nti.GetEntry(DE_CAMERA_CONFIG_ENTRY("4")).GetValue();
    assert(value);
    Camera<4>::SetConfig(new PipelineConfig(value));  // ownership passed

    value = nti.GetEntry(DE_STREAM_CONFIG_ENTRY("4")).GetValue();
    assert(value);
    Camera<4>::SetStream(new StreamConfig(value));  // ownership passed
  }
}

/**
 * Nt2spdlogLevel converts logging levels.
 */
spdlog::level::level_enum Nt2spdlogLevel(const nt::LogMessage& msg) {
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
