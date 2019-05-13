#pragma once

#include <spdlog/spdlog.h>
#include <atomic>
#include <future>
#include <memory>
#include <tinyfsm.hpp>
#include "default_pipeline.hpp"
#include "events.hpp"
#include "lights.hpp"

namespace deadeye {

// ---------------------------------------------------------------------------
// Events
//
struct CameraOn : tinyfsm::Event {};
struct CameraOff : tinyfsm::Event {};

// ---------------------------------------------------------------------------
// Camera FSM
//
template <int inum>
class Camera : public tinyfsm::Fsm<Camera<inum>> {
  friend class tinyfsm::Fsm<Camera<inum>>;
  friend fmt::formatter<Camera<inum>>;

 private:
  void react(tinyfsm::Event const &) {}  // default

  virtual void react(CameraOn const &) {}
  virtual void react(CameraOff const &) {}

  virtual void entry() = 0;
  void exit() {}

 protected:
  static DefaultPipeline pipeline_;
  static std::future<void> pipeline_future_;
  static std::string error_;
};

// state variable definitions
template <int inum>
DefaultPipeline Camera<inum>::pipeline_{inum};

template <int inum>
std::future<void> Camera<inum>::pipeline_future_;

template <int inum>
std::string Camera<inum>::error_;

}  // namespace deadeye
