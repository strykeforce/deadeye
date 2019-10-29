#pragma once

#include <fmt/core.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>
#include <spdlog/spdlog.h>
#include <atomic>
#include <future>
#include <memory>
#include <tinyfsm.hpp>

#include "config/capture_config.h"
#include "config/deadeye_config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "pipeline/pipeline.h"

namespace deadeye {

template <class T>
using owner = T;

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
    pipeline_ = std::move(pipeline);
  }
  static void Initialize(CaptureConfig const &cc, PipelineConfig const &pc,
                         StreamConfig const &sc) {
    pipeline_->ConfigCapture(cc);
    pipeline_->ConfigPipeline(pc);
    pipeline_->ConfigStream(sc);
  }

 private:
  void react(tinyfsm::Event const &) {}  // default

  virtual void react(CameraOn const &) {}
  virtual void react(CameraOff const &) {}
  virtual void react(ConfigCapture const &c) {
    Camera<inum>::pipeline_->ConfigCapture(c.config);
  }
  virtual void react(ConfigPipeline const &c) {
    Camera<inum>::pipeline_->ConfigPipeline(c.config);
  }
  virtual void react(ConfigStream const &s) {
    Camera<inum>::pipeline_->ConfigStream(s.config);
  }

  virtual void entry() = 0;
  virtual void exit() = 0;

 protected:
  static std::unique_ptr<Pipeline> pipeline_;
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
std::unique_ptr<Pipeline> Camera<inum>::pipeline_;

template <int inum>
std::future<void> Camera<inum>::pipeline_future_;

template <int inum>
std::atomic<bool> Camera<inum>::has_error_{false};

template <int inum>
std::string Camera<inum>::error_;

}  // namespace deadeye
