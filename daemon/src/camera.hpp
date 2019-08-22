#pragma once

#include <spdlog/spdlog.h>
#include <atomic>
#include <future>
#include <memory>
#include <tinyfsm.hpp>

#include "pipeline/pipeline.hpp"
#include "stream_config.hpp"

namespace deadeye {

// ---------------------------------------------------------------------------
// Events
//
struct CameraOn : tinyfsm::Event {};
struct CameraOff : tinyfsm::Event {};
struct ConfigCamera : tinyfsm::Event {
  PipelineConfig *config;
};
struct ConfigStream : tinyfsm::Event {
  StreamConfig *config;
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
  static void SetConfig(PipelineConfig *config) {
    pipeline_->UpdateConfig(config);
  }
  static void SetStream(StreamConfig *config) {
    pipeline_->UpdateStream(config);
  }

 private:
  void react(tinyfsm::Event const &) {}  // default

  virtual void react(CameraOn const &) {}
  virtual void react(CameraOff const &) {}
  virtual void react(ConfigCamera const &c) {
    Camera<inum>::pipeline_->UpdateConfig(c.config);
  }
  virtual void react(ConfigStream const &s) {
    Camera<inum>::pipeline_->UpdateStream(s.config);
  }

  virtual void entry() = 0;
  virtual void exit() = 0;

 protected:
  static std::unique_ptr<Pipeline> pipeline_;
  static std::future<void> pipeline_future_;
  static std::atomic<bool> has_error_;
  static std::string error_;
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
