#pragma once

#include <tinyfsm.hpp>

namespace deadeye {
// ---------------------------------------------------------------------------
// Events
//
struct LightsOn : tinyfsm::Event {};
struct LightsOff : tinyfsm::Event {};

// ---------------------------------------------------------------------------
// Lights FSM
//
class Lights : public tinyfsm::Fsm<Lights> {
  friend class tinyfsm::Fsm<Lights>;

 private:
  // default reaction for unhandled events
  void react(tinyfsm::Event const &) {}

  virtual void react(LightsOn const &) {}
  virtual void react(LightsOff const &) {}

  virtual void entry() = 0;
  void exit() {}
};
}  // namespace deadeye
