#pragma once

#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>
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
  virtual void exit() = 0;

 protected:
  static std::future<void> task_future_;
  static std::atomic<bool> cancel_task_;
  static std::string nt_path_;

  void SetStatus(std::string name, bool state) {
    std::string path = nt_path_ + name;
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
std::string Lights<inum>::nt_path_ =
    std::string(DE_CONTROL_TABLE) + std::string("/") + std::to_string(inum) +
    std::string(DE_LIGHTS) + std::string("/");

}  // namespace deadeye
