#pragma once

#include <tinyfsm.hpp>
#include "events.hpp"

namespace deadeye {
// ---------------------------------------------------------------------------
// Events
//
struct LightsOn : tinyfsm::Event {};
struct LightsOff : tinyfsm::Event {};

// ---------------------------------------------------------------------------
// Lights FSM
//
template <int inum>
class Lights : public tinyfsm::Fsm<Lights<inum>> {
  friend class tinyfsm::Fsm<Lights<inum>>;

 private:
  // default reaction for unhandled events
  void react(tinyfsm::Event const &) {}

  virtual void react(LightsOn const &) {}
  virtual void react(LightsOff const &) {}

  virtual void entry() = 0;
  void exit() {}
};
}  // namespace deadeye
