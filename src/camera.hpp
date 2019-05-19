#pragma once

#include <spdlog/spdlog.h>
#include <atomic>
#include <future>
#include <memory>
#include <tinyfsm.hpp>

#include "pipeline.hpp"

namespace deadeye {

// ---------------------------------------------------------------------------
// Events
//
struct CameraOn : tinyfsm::Event {};
struct CameraOff : tinyfsm::Event {};
struct CameraConfig : tinyfsm::Event {
  PipelineConfig config;
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

 private:
  void react(tinyfsm::Event const &) {}  // default

  virtual void react(CameraOn const &) {}
  virtual void react(CameraOff const &) {}
  virtual void react(CameraConfig const &c) {
    Camera<inum>::pipeline_->UpdateConfig(c.config);
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
std::unique_ptr<Pipeline> Camera<inum>::pipeline_ =
    std::make_unique<TestPatternPipeline>(inum);

template <int inum>
std::future<void> Camera<inum>::pipeline_future_;

template <int inum>
std::atomic<bool> Camera<inum>::has_error_{false};

template <int inum>
std::string Camera<inum>::error_;

}  // namespace deadeye
