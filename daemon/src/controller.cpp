// Copyright (c) 2022 Stryke Force FRC 2767

#include "controller.h"

#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>
#include <ntcore_cpp.h>
#include <spdlog/spdlog.h>

#include <atomic>
#include <cassert>
#include <csignal>
#include <future>
#include <memory>
#include <utility>

#include "config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "hardware/camera.h"
#include "hardware/lights.h"
#include "link/link.h"
#include "log/client_logger.h"
#include "pipeline/null_pipeline.h"

// forward declaration
static spdlog::level::level_enum Nt2spdlogLevel(const nt::LogMessage& msg);

using ::deadeye::Controller;

namespace {
constexpr double kPollTimeout = 0.25;

#ifdef __linux__
constexpr std::string_view kNetworkTablesIniPath =
    "/etc/opt/deadeye/networktables.ini";
#else
constexpr std::string_view kNetworkTablesIniPath = "networktables.ini";
#endif

std::atomic<bool> quit{false};

void signal_handler([[maybe_unused]] int signal) { quit = true; }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
/**
 * From http://www.rioki.org/2016/03/31/cpp-switch-string.html
 */
constexpr unsigned int hash(const char* str, int h = 0) {
  return str[h] == 0 ? 5381 : (hash(str, h + 1) * 33) ^ str[h];
}
#pragma clang diagnostic pop
}  // namespace

/**
 * Constructor for Controller.
 */
Controller::Controller(Pipelines* pipelines)
    : inst_{0}, poller_{0}, entry_listener_{0}, has_active_pipeline_{} {
  assert(pipelines);

  StartNetworkTables();

  for (int i = 0; i < static_cast<int>(pipelines->size()); i++) {
    if (!(*pipelines)[i]) {
      (*pipelines)[i] = std::make_unique<NullPipeline>(i);
      has_active_pipeline_[i] = false;
    } else {
      has_active_pipeline_[i] = true;
      ClientLogger client_logger{i};
      client_logger.Info(fmt::format("{}<{}>: Deadeye version {}",
                                     (*pipelines)[i]->GetName(), CameraId(i),
                                     GetDeadeyeVersion()));
    }
    if ((*pipelines)[i]->GetInum() != i) {
      spdlog::critical("{} does not match position in initialization array: {}",
                       *(*pipelines)[i], i);
      throw std::runtime_error("critical error");
    }
  }

  constexpr std::string_view kSignalHandlerError =
      "error installing {} signal handler";
  if (SIG_ERR == std::signal(SIGINT, signal_handler))
    spdlog::critical(kSignalHandlerError, "SIGINT");
  if (SIG_ERR == std::signal(SIGTERM, signal_handler))
    spdlog::critical(kSignalHandlerError, "SIGTERM");

  Camera<0>::SetPipeline(std::move((*pipelines)[0]));
  Camera<1>::SetPipeline(std::move((*pipelines)[1]));
  Camera<2>::SetPipeline(std::move((*pipelines)[2]));
  Camera<3>::SetPipeline(std::move((*pipelines)[3]));
  Camera<4>::SetPipeline(std::move((*pipelines)[4]));

  InitializeNetworkTables();

  InitializeCamera<0>();
  InitializeCamera<1>();
  InitializeCamera<2>();
  InitializeCamera<3>();
  InitializeCamera<4>();

  LogCamera<0>();
  LogCamera<1>();
  LogCamera<2>();
  LogCamera<3>();
  LogCamera<4>();

  StartPoller();
}

/**
 * Destructor for Controller.
 */
Controller::~Controller() {
  try {
    nt::Flush(inst_);
    nt::DestroyEntryListenerPoller(poller_);
  } catch (const std::exception& e) {
    spdlog::error("~Controller: {}", e.what());
  }
  spdlog::info("Deadeye Controller exiting.");
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-function-cognitive-complexity"
/**
 * Run listens for commands and config changes.
 */
void Controller::Run() {
  if (has_active_pipeline_[0]) {
    Camera<0>::start();
    Lights<0>::start();
  }
  if (has_active_pipeline_[1]) {
    Camera<1>::start();
    Lights<1>::start();
  }
  if (has_active_pipeline_[2]) {
    Camera<2>::start();
    Lights<2>::start();
  }
  if (has_active_pipeline_[3]) {
    Camera<3>::start();
    Lights<3>::start();
  }
  if (has_active_pipeline_[4]) {
    Camera<4>::start();
    Lights<4>::start();
  }

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
          assert(has_active_pipeline_[0]);
          if (entry.value->GetBoolean()) Camera<0>::dispatch(CameraOn());
          break;
        case hash(DE_CAMERA_CONTROL("0", DE_OFF)):
          assert(has_active_pipeline_[0]);
          if (entry.value->GetBoolean()) Camera<0>::dispatch(CameraOff());
          break;
        case hash(DE_CAMERA_CONTROL("0", DE_ERROR)):
          break;
        case hash(DE_LIGHTS_CONTROL("0", DE_ON)):
          assert(has_active_pipeline_[0]);
          if (entry.value->GetBoolean()) Lights<0>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("0", DE_BLINK)):
          assert(has_active_pipeline_[0]);
          if (entry.value->GetBoolean()) Lights<0>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("0", DE_OFF)):
          assert(has_active_pipeline_[0]);
          if (entry.value->GetBoolean()) Lights<0>::dispatch(LightsOff());
          break;
        case hash(DE_CAPTURE_CONFIG_ENTRY("0")): {
          assert(has_active_pipeline_[0]);
          ConfigCapture event;
          event.config = CaptureConfig{entry.value};
          spdlog::debug("Camera<0>: {}", event.config);
          Camera<0>::dispatch(event);
          break;
        }
        case hash(DE_PIPELINE_CONFIG_ENTRY("0")): {
          assert(has_active_pipeline_[0]);
          ConfigPipeline event;
          event.config = PipelineConfig{entry.value};
          spdlog::debug("Camera<0>: {}", event.config);
          Camera<0>::dispatch(event);
          break;
        }
        case hash(DE_STREAM_CONFIG_ENTRY("0")): {
          ConfigStream event;
          event.config = StreamConfig{entry.value};
          spdlog::debug("Camera<0>: {}", event.config);
          Camera<0>::dispatch(event);
          break;
        }

        case hash(DE_LOG_ENTRY("0")):
          break;

        //
        // Camera 1 events
        //
        case hash(DE_CAMERA_CONTROL("1", DE_ON)):
          assert(has_active_pipeline_[1]);
          if (entry.value->GetBoolean()) Camera<1>::dispatch(CameraOn());
          break;
        case hash(DE_CAMERA_CONTROL("1", DE_OFF)):
          assert(has_active_pipeline_[1]);
          if (entry.value->GetBoolean()) Camera<1>::dispatch(CameraOff());
          break;
        case hash(DE_CAMERA_CONTROL("1", DE_ERROR)):
          break;
        case hash(DE_LIGHTS_CONTROL("1", DE_ON)):
          assert(has_active_pipeline_[1]);
          if (entry.value->GetBoolean()) Lights<1>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("1", DE_BLINK)):
          assert(has_active_pipeline_[1]);
          if (entry.value->GetBoolean()) Lights<1>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("1", DE_OFF)):
          assert(has_active_pipeline_[1]);
          if (entry.value->GetBoolean()) Lights<1>::dispatch(LightsOff());
          break;
        case hash(DE_CAPTURE_CONFIG_ENTRY("1")): {
          assert(has_active_pipeline_[1]);
          ConfigCapture event;
          event.config = CaptureConfig{entry.value};
          spdlog::debug("Camera<1>: {}", event.config);
          Camera<1>::dispatch(event);
          break;
        }
        case hash(DE_PIPELINE_CONFIG_ENTRY("1")): {
          assert(has_active_pipeline_[1]);
          ConfigPipeline event;
          event.config = PipelineConfig{entry.value};
          spdlog::debug("Camera<1>: {}", event.config);
          Camera<1>::dispatch(event);
          break;
        }
        case hash(DE_STREAM_CONFIG_ENTRY("1")): {
          assert(has_active_pipeline_[1]);
          ConfigStream event;
          event.config = StreamConfig{entry.value};
          spdlog::debug("Camera<1>: {}", event.config);
          Camera<1>::dispatch(event);
          break;
        }

        case hash(DE_LOG_ENTRY("1")):
          break;

        //
        // Camera 2 events
        //
        case hash(DE_CAMERA_CONTROL("2", DE_ON)):
          assert(has_active_pipeline_[2]);
          if (entry.value->GetBoolean()) Camera<2>::dispatch(CameraOn());
          break;
        case hash(DE_CAMERA_CONTROL("2", DE_OFF)):
          assert(has_active_pipeline_[2]);
          if (entry.value->GetBoolean()) Camera<2>::dispatch(CameraOff());
          break;
        case hash(DE_CAMERA_CONTROL("2", DE_ERROR)):
          break;
        case hash(DE_LIGHTS_CONTROL("2", DE_ON)):
          assert(has_active_pipeline_[2]);
          if (entry.value->GetBoolean()) Lights<2>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("2", DE_BLINK)):
          assert(has_active_pipeline_[2]);
          if (entry.value->GetBoolean()) Lights<2>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("2", DE_OFF)):
          assert(has_active_pipeline_[2]);
          if (entry.value->GetBoolean()) Lights<2>::dispatch(LightsOff());
          break;
        case hash(DE_CAPTURE_CONFIG_ENTRY("2")): {
          assert(has_active_pipeline_[2]);
          ConfigCapture event;
          event.config = CaptureConfig{entry.value};
          spdlog::debug("Camera<2>: {}", event.config);
          Camera<2>::dispatch(event);
          break;
        }
        case hash(DE_PIPELINE_CONFIG_ENTRY("2")): {
          assert(has_active_pipeline_[2]);
          ConfigPipeline event;
          event.config = PipelineConfig{entry.value};
          spdlog::debug("Camera<2>: {}", event.config);
          Camera<2>::dispatch(event);
          break;
        }
        case hash(DE_STREAM_CONFIG_ENTRY("2")): {
          assert(has_active_pipeline_[2]);
          ConfigStream event;
          event.config = StreamConfig{entry.value};
          spdlog::debug("Camera<2>: {}", event.config);
          Camera<2>::dispatch(event);
          break;
        }

        case hash(DE_LOG_ENTRY("2")):
          break;

        //
        // Camera 3 events
        //
        case hash(DE_CAMERA_CONTROL("3", DE_ON)):
          assert(has_active_pipeline_[3]);
          if (entry.value->GetBoolean()) Camera<3>::dispatch(CameraOn());
          break;
        case hash(DE_CAMERA_CONTROL("3", DE_OFF)):
          assert(has_active_pipeline_[3]);
          if (entry.value->GetBoolean()) Camera<3>::dispatch(CameraOff());
          break;
        case hash(DE_CAMERA_CONTROL("3", DE_ERROR)):
          break;
        case hash(DE_LIGHTS_CONTROL("3", DE_ON)):
          assert(has_active_pipeline_[3]);
          if (entry.value->GetBoolean()) Lights<3>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("3", DE_BLINK)):
          assert(has_active_pipeline_[3]);
          if (entry.value->GetBoolean()) Lights<3>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("3", DE_OFF)):
          assert(has_active_pipeline_[3]);
          if (entry.value->GetBoolean()) Lights<3>::dispatch(LightsOff());
          break;
        case hash(DE_CAPTURE_CONFIG_ENTRY("3")): {
          assert(has_active_pipeline_[3]);
          ConfigCapture event;
          event.config = CaptureConfig{entry.value};
          spdlog::debug("Camera<3>: {}", event.config);
          Camera<3>::dispatch(event);
          break;
        }
        case hash(DE_PIPELINE_CONFIG_ENTRY("3")): {
          assert(has_active_pipeline_[3]);
          ConfigPipeline event;
          event.config = PipelineConfig{entry.value};
          spdlog::debug("Camera<3>: {}", event.config);
          Camera<3>::dispatch(event);
          break;
        }
        case hash(DE_STREAM_CONFIG_ENTRY("3")): {
          assert(has_active_pipeline_[3]);
          ConfigStream event;
          event.config = StreamConfig{entry.value};
          spdlog::debug("Camera<3>: {}", event.config);
          Camera<3>::dispatch(event);
          break;
        }

        case hash(DE_LOG_ENTRY("3")):
          break;

        //
        // Camera 4 events
        //
        case hash(DE_CAMERA_CONTROL("4", DE_ON)):
          assert(has_active_pipeline_[4]);
          if (entry.value->GetBoolean()) Camera<4>::dispatch(CameraOn());
          break;
        case hash(DE_CAMERA_CONTROL("4", DE_OFF)):
          assert(has_active_pipeline_[4]);
          if (entry.value->GetBoolean()) Camera<4>::dispatch(CameraOff());
          break;
        case hash(DE_CAMERA_CONTROL("4", DE_ERROR)):
          break;
        case hash(DE_LIGHTS_CONTROL("4", DE_ON)):
          assert(has_active_pipeline_[4]);
          if (entry.value->GetBoolean()) Lights<4>::dispatch(LightsOn());
          break;
        case hash(DE_LIGHTS_CONTROL("4", DE_BLINK)):
          assert(has_active_pipeline_[4]);
          if (entry.value->GetBoolean()) Lights<4>::dispatch(LightsBlink());
          break;
        case hash(DE_LIGHTS_CONTROL("4", DE_OFF)):
          assert(has_active_pipeline_[4]);
          if (entry.value->GetBoolean()) Lights<4>::dispatch(LightsOff());
          break;
        case hash(DE_CAPTURE_CONFIG_ENTRY("4")): {
          assert(has_active_pipeline_[4]);
          ConfigCapture event;
          event.config = CaptureConfig{entry.value};
          spdlog::debug("Camera<4>: {}", event.config);
          Camera<4>::dispatch(event);
          break;
        }
        case hash(DE_PIPELINE_CONFIG_ENTRY("4")): {
          assert(has_active_pipeline_[4]);
          ConfigPipeline event;
          event.config = PipelineConfig{entry.value};
          spdlog::debug("Camera<4>: {}", event.config);
          Camera<4>::dispatch(event);
          break;
        }
        case hash(DE_STREAM_CONFIG_ENTRY("4")): {
          assert(has_active_pipeline_[4]);
          ConfigStream event;
          event.config = StreamConfig{entry.value};
          spdlog::debug("Camera<4>: {}", event.config);
          Camera<4>::dispatch(event);
          break;
        }

        case hash(DE_LOG_ENTRY("4")):
          break;

        case hash(DE_LINK_ENTRY):
          spdlog::debug("Deadeye: {}", entry.value);
          break;

        default:
          spdlog::warn("Controller: {} event unrecognized in {}, line {}",
                       entry.name, __FILE__, __LINE__);
          break;
      }
    }
  }
}
#pragma clang diagnostic pop

void Controller::ShutDown() {
  nt::DestroyEntryListenerPoller(poller_);
  nt::RemoveEntryListener(entry_listener_);

  // fsm dispatches to all instances
  if (has_active_pipeline_[0]) {
    Camera<0>::dispatch(CameraOff());
    Lights<0>::dispatch(LightsOff());
    ClientLogger client_logger{0};
    client_logger.Warn(fmt::format("Camera<{}> shutting down", CameraId(0)));
  }
  if (has_active_pipeline_[1]) {
    Camera<1>::dispatch(CameraOff());
    Lights<1>::dispatch(LightsOff());
    ClientLogger client_logger{1};
    client_logger.Warn(fmt::format("Camera<{}> shutting down", CameraId(1)));
  }
  if (has_active_pipeline_[2]) {
    Camera<2>::dispatch(CameraOff());
    Lights<2>::dispatch(LightsOff());
    ClientLogger client_logger{2};
    client_logger.Warn(fmt::format("Camera<{}> shutting down", CameraId(2)));
  }
  if (has_active_pipeline_[3]) {
    Camera<3>::dispatch(CameraOff());
    Lights<3>::dispatch(LightsOff());
    ClientLogger client_logger{3};
    client_logger.Warn(fmt::format("Camera<{}> shutting down", CameraId(3)));
  }
  if (has_active_pipeline_[4]) {
    Camera<4>::dispatch(CameraOff());
    Lights<4>::dispatch(LightsOff());
    ClientLogger client_logger{4};
    client_logger.Warn(fmt::format("Camera<{}> shutting down", CameraId(4)));
  }
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
      0, std::numeric_limits<unsigned int>::max());

  char* env_nt_server = std::getenv("DEADEYE_NT_SERVER");
  const char* nt_server = env_nt_server != nullptr ? env_nt_server : NT_SERVER;

  char* env_nt_port = std::getenv("DEADEYE_NT_PORT");
  const unsigned int nt_port =
      env_nt_port != nullptr ? static_cast<unsigned int>(std::stoi(env_nt_port))
                             : NT_DEFAULT_PORT;

  // create own NT server if DEADEYE_NT_SERVER=127.0.0.1
  if (std::strncmp("127.0.0.1", nt_server, 15) == 0) {
    spdlog::info("Starting our own NetworkTables server");
    spdlog::info("NetworkTables persist file: {}", kNetworkTablesIniPath);
    nt::StartServer(inst_, kNetworkTablesIniPath, "0.0.0.0", nt_port);
    return;
  }

  // else connect to server at DEADEYE_NT_SERVER and wait for connection
  std::promise<void> barrier;
  std::future<void> barrier_future = barrier.get_future();

  auto conn_listener = nt::AddConnectionListener(
      inst_, [&](auto /*event*/) mutable { barrier.set_value(); }, true);

  spdlog::info("Starting NetworkTables client connecting to {}", nt_server);
  nt::StartClient(inst_, nt_server, nt_port);

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

/**
 * InitializeNetworkTables sets up default values in network tables.
 */
void Controller::InitializeNetworkTables() {
  auto nti = nt::NetworkTableInstance(inst_);

  for (int i = 0; i < static_cast<int>(has_active_pipeline_.size()); i++) {
    if (!has_active_pipeline_[i]) continue;

    auto table = nti.GetTable(CameraControlTablePath(i));
    table->PutBoolean(DE_ON, false);
    table->PutBoolean(DE_OFF, true);
    table->PutBoolean(DE_ERROR, false);

    table = nti.GetTable(LightsControlTablePath(i));
    table->PutBoolean(DE_ON, false);
    table->PutBoolean(DE_OFF, true);
    table->PutBoolean(DE_BLINK, false);

    auto entry = nti.GetEntry(CaptureConfigEntryPath(i));
    CaptureConfig cc{CaptureType::test, 320, 180, 60, json::object()};
    json j = cc;
    entry.SetDefaultString(j.dump());
    entry.SetPersistent();

    entry = nti.GetEntry(PipelineConfigEntryPath(i));
    PipelineConfig pc{0,        {0, 255},       {0, 255},
                      {0, 255}, FilterConfig(), FrameLogConfig()};
    j = pc;
    entry.SetDefaultString(j.dump());
    entry.SetPersistent();

    entry = nti.GetEntry(StreamConfigEntryPath(i));
    StreamConfig sc{i};
    j = sc;
    entry.SetString(j.dump());
  }

  Link::Init(nti);
}

template <int inum>
void Controller::InitializeCamera() {
  if (!has_active_pipeline_[inum]) return;

  auto nti = nt::NetworkTableInstance(inst_);
  CaptureConfig cc{nti.GetEntry(CaptureConfigEntryPath(inum)).GetValue()};
  PipelineConfig pc{nti.GetEntry(PipelineConfigEntryPath(inum)).GetValue()};
  StreamConfig sc{nti.GetEntry(StreamConfigEntryPath(inum)).GetValue()};
  Camera<inum>::Initialize(cc, pc, sc);

  json j;
  j["pipeline"] = Camera<inum>::GetPipeline()->GetName();
  j["version"] = GetDeadeyeVersion();
  j["logging"] = false;
  auto entry = nti.GetEntry(InfoEntryPath(inum));
  entry.SetString(j.dump());
  entry.ClearPersistent();

  spdlog::debug("Camera<{}> initialized", CameraId(inum));
}

template <int inum>
void Controller::LogCamera() {
  auto pl = Camera<inum>::GetPipeline();
  spdlog::info("Camera<{}>: {} {}", CameraId(inum), *pl,
               has_active_pipeline_[inum] ? "[active]" : "");
}

/**
 * Nt2spdlogLevel converts logging levels.
 */
spdlog::level::level_enum Nt2spdlogLevel(const nt::LogMessage& msg) {
  switch (msg.level) {
    case NT_LOG_CRITICAL:
      return ::spdlog::level::critical;
    case NT_LOG_ERROR:
      return ::spdlog::level::err;
    case NT_LOG_WARNING:
      return ::spdlog::level::warn;
    case NT_LOG_INFO:
      return ::spdlog::level::info;
    default:
      return ::spdlog::level::debug;
  }
}
