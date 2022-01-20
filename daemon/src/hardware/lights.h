#pragma once

#include <fmt/core.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>

#include <atomic>
#include <future>
#include <tinyfsm.hpp>

#include "config/deadeye_config.h"
#include "led_drive.h"

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
  [[maybe_unused]] void react(tinyfsm::Event const &) {}

  [[maybe_unused]] virtual void react(LightsOn const &) {}
  [[maybe_unused]] virtual void react(LightsOff const &) {}
  [[maybe_unused]] virtual void react(LightsBlink const &) {}

  [[maybe_unused]] virtual void entry() = 0;
  virtual void exit() = 0;

 protected:
  static std::future<void> task_future_;
  static std::atomic<bool> cancel_task_;
  static LedDrive led_;

  void SetStatus(std::string name, bool state) {
    std::string path = fmt::format("{}/{}", LightsControlTablePath(inum), name);
    auto entry = nt::GetEntry(nt::GetDefaultInstance(), path);
    nt::SetEntryValue(entry, nt::Value::MakeBoolean(state));
  }
};

// state variable definitions
template <int inum>
std::future<void> Lights<inum>::task_future_;

template <int inum>
std::atomic<bool> Lights<inum>::cancel_task_{false};

template <int inum>
LedDrive Lights<inum>::led_{inum}; // NOLINT

}  // namespace deadeye
