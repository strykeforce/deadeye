// Copyright (c) 2022 Stryke Force FRC 2767

#include "lights.h"

#include <spdlog/spdlog.h>

#include <future>
#include <thread>
#include <tinyfsm.hpp>

#include "camera.h"

namespace {
using namespace std::literals::  // NOLINT(build/namespaces_literals)
    chrono_literals;
constexpr auto kBlinkPeriod = 250ms;
}  // namespace

// ---------------------------------------------------------------------------
// Lights states
//
namespace deadeye::lights {
template <int inum>
class Off;
template <int inum>
class Blinking;

template <int inum>
class On : public Lights<inum> {
  using base = Lights<inum>;

  void entry() override {
    base::SetStatus(DE_ON, true);
    spdlog::info("Lights<{}> on", CameraId(inum));
    base::led_.On();
    base::on_ = true;
  }

  void react(LightsBlink const&) override {
    base::template transit<lights::Blinking<inum>>();
  }

  void react(LightsOff const&) override {
    Camera<inum>::dispatch(LightsOff());
    base::template transit<lights::Off<inum>>();
  }

  void exit() override { base::SetStatus(DE_ON, false); }
};

template <int inum>
class Blinking : public Lights<inum> {
  using base = Lights<inum>;

  void entry() override {
    base::SetStatus(DE_BLINK, true);

    base::cancel_task_ = false;
    base::task_future_ = std::async(std::launch::async, [] {
      while (true) {
        spdlog::trace("Lights<{}> on", CameraId(inum));
        base::led_.On();
        std::this_thread::sleep_for(kBlinkPeriod);
        if (base::cancel_task_.load()) break;

        spdlog::trace("Lights<{}> off", CameraId(inum));
        base::led_.Off();
        std::this_thread::sleep_for(kBlinkPeriod);
        if (base::cancel_task_.load()) break;
      }
    });
    spdlog::info("Lights<{}> blink", CameraId(inum));
  }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCallsOfFunction"
  void CancelTask() {
    base::cancel_task_ = true;
    if (base::task_future_.valid()) {
      try {
        base::task_future_.get();
      } catch (std::exception const& e) {
        spdlog::error("Lights<{}> error while cancelling blink: {}",
                      CameraId(inum), e.what());
      }
    }
  }
#pragma clang diagnostic pop

  void react(LightsOn const&) override {
    CancelTask();
    base::template transit<lights::On<inum>>();
  }

  void react(LightsOff const&) override {
    CancelTask();
    base::template transit<lights::Off<inum>>();
  }

  void exit() override { base::SetStatus(DE_BLINK, false); }
};

template <int inum>
class Off : public Lights<inum> {
  using base = Lights<inum>;

  void entry() override {
    base::SetStatus(DE_OFF, true);
    spdlog::info("Lights<{}> off", CameraId(inum));
    base::led_.Off();
    base::on_ = false;
  }

  void react(LightsOn const&) override {
    Camera<inum>::dispatch(LightsOn());
    base::template transit<lights::On<inum>>();
  }

  void react(LightsBlink const&) override {
    base::template transit<lights::Blinking<inum>>();
  }

  void exit() override { base::SetStatus(DE_OFF, false); }
};
}  // namespace deadeye::lights

FSM_INITIAL_STATE(deadeye::Lights<0>, deadeye::lights::Off<0>)
FSM_INITIAL_STATE(deadeye::Lights<1>, deadeye::lights::Off<1>)
FSM_INITIAL_STATE(deadeye::Lights<2>, deadeye::lights::Off<2>)
FSM_INITIAL_STATE(deadeye::Lights<3>, deadeye::lights::Off<3>)
FSM_INITIAL_STATE(deadeye::Lights<4>, deadeye::lights::Off<4>)
