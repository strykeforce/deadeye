#pragma once

#include <atomic>
#include <future>
#include <tinyfsm.hpp>

namespace deadeye {
// ---------------------------------------------------------------------------
// Events
//
struct LightsOn : tinyfsm::Event {};
struct LightsOff : tinyfsm::Event {};
struct LightsBlink : tinyfsm::Event {};

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
  virtual void react(LightsBlink const &) {}

  virtual void entry() = 0;
  void exit() {}

 protected:
  static std::future<void> blink_task_future_;
  static std::atomic<bool> cancel_;
};

// state variable definitions
template <int inum>
std::future<void> Lights<inum>::blink_task_future_;

template <int inum>
std::atomic<bool> Lights<inum>::cancel_{false};

}  // namespace deadeye
