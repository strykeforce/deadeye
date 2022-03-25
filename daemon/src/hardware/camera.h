#pragma once

#include <fmt/core.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>
#include <spdlog/spdlog.h>

#include <atomic>
#include <future>
#include <memory>
#include <tinyfsm.hpp>

#include "config.h"
#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "pipeline/runner.h"

namespace deadeye {

class Pipeline;

template <class T>
using owner [[maybe_unused]] = T;

// ---------------------------------------------------------------------------
// Events
//
struct CameraOn : tinyfsm::Event {};
struct CameraOff : tinyfsm::Event {};
struct ConfigCapture : tinyfsm::Event {
  CaptureConfig config;
};
struct ConfigPipeline : tinyfsm::Event {
  PipelineConfig config;
};
struct ConfigStream : tinyfsm::Event {
  StreamConfig config;
};

// ---------------------------------------------------------------------------
// Camera FSM
//
template <int inum>
class Camera : public tinyfsm::Fsm<Camera<inum>> {
  friend class tinyfsm::Fsm<Camera<inum>>;

 public:
  static bool HasError() { return has_error_.load(); }

  static void SetPipeline(std::unique_ptr<Pipeline> pipeline) {
    pipeline_runner_.SetPipeline(std::move(pipeline));
  }

  static Pipeline* GetPipeline() { return pipeline_runner_.GetPipeline(); }

  static void Initialize(CaptureConfig const& cc, PipelineConfig const& pc,
                         StreamConfig const& sc) {
    pipeline_runner_.Configure(cc);
    pipeline_runner_.Configure(pc);
    pipeline_runner_.Configure(sc);
  }

 private:
  [[maybe_unused]] void react(tinyfsm::Event const&) {}

  [[maybe_unused]] virtual void react(CameraOn const&) {}
  [[maybe_unused]] virtual void react(CameraOff const&) {}
  [[maybe_unused]] virtual void react(ConfigCapture const& c) {
    Camera<inum>::pipeline_runner_.Configure(c.config);
  }
  [[maybe_unused]] virtual void react(ConfigPipeline const& c) {
    Camera<inum>::pipeline_runner_.Configure(c.config);
  }
  [[maybe_unused]] virtual void react(ConfigStream const& s) {
    Camera<inum>::pipeline_runner_.Configure(s.config);
  }

  [[maybe_unused]] virtual void entry() = 0;
  virtual void exit() = 0;

 protected:
  static Runner pipeline_runner_;
  static std::future<void> pipeline_future_;
  static std::atomic<bool> has_error_;
  static std::string error_;

  void SetStatus(std::string name, bool state) {
    std::string path = fmt::format("{}/{}", CameraControlTablePath(inum), name);
    auto entry = nt::GetEntry(nt::GetDefaultInstance(), path);
    nt::SetEntryValue(entry, nt::Value::MakeBoolean(state));
  }
};

// state variable definitions
template <int inum>
Runner Camera<inum>::pipeline_runner_; // NOLINT(cert-err58-cpp)

template <int inum>
std::future<void> Camera<inum>::pipeline_future_;

template <int inum>
std::atomic<bool> Camera<inum>::has_error_{false};

template <int inum>
std::string Camera<inum>::error_;

}  // namespace deadeye
