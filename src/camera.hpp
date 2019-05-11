#pragma once

#include <spdlog/spdlog.h>
#include <atomic>
#include <memory>
#include <sstream>
#include <thread>
#include <tinyfsm.hpp>
#include "events.hpp"
#include "fmt/format.h"
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

  //  Camera(const Camera &) = delete;
  //  Camera &operator=(const Camera &) = delete;

 private:
  // default reaction for unhandled events
  void react(tinyfsm::Event const &) {}

  virtual void react(CameraOn const &) {}
  virtual void react(CameraOff const &) {}
  virtual void react(ShutDown const &) {}

  virtual void entry() = 0;
  void exit() {}

 protected:
  static std::unique_ptr<std::thread> pipeline_thread_;
  static std::atomic<bool> quit_;
};

// state variable definitions
template <int inum>
std::unique_ptr<std::thread> Camera<inum>::pipeline_thread_{};

template <int inum>
std::atomic<bool> Camera<inum>::quit_{false};

}  // namespace deadeye

// custom formatting for fmt library
template <int inum>
struct fmt::formatter<deadeye::Camera<inum>> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const deadeye::Camera<inum> &c, FormatContext &ctx) {
    auto myid = std::this_thread::get_id();
    std::stringstream ss;
    ss << myid;
    return format_to(ctx.out(), "Camera ({})", ss.str());
  }
};
