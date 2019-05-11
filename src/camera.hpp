#pragma once

#include <spdlog/spdlog.h>
#include <tinyfsm.hpp>
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

 private:
  // default reaction for unhandled events
  void react(tinyfsm::Event const &) {}

  virtual void react(CameraOn const &) {}
  virtual void react(CameraOff const &) {}
  virtual void react(ShutDown const &) {}

  virtual void entry() = 0;
  void exit() {}
};

}  // namespace deadeye