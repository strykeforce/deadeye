#include <spdlog/spdlog.h>
#include <future>
#include <thread>
#include <tinyfsm.hpp>
#include "controller.hpp"
#include "lights.hpp"
#include "nt_constants.hpp"

using namespace deadeye;

namespace {
using namespace std::literals::chrono_literals;
constexpr auto kBlinkPeriod = 250ms;
}  // namespace

// ---------------------------------------------------------------------------
// Lights states
//
namespace lights {
template <int inum>
class Off;
template <int inum>
class Blinking;

template <int inum>
class On : public Lights<inum> {
  void entry() override {
    auto &controller = Controller::GetInstance();
    controller.SetLightsStatus(inum, DE_ON, true);
    controller.SetLightsStatus(inum, DE_OFF, false);
    controller.SetLightsStatus(inum, DE_BLINK, false);
    spdlog::info("Lights<{}> on", inum);
  }

  void react(LightsBlink const &) override {
    Lights<inum>::template transit<lights::Blinking<inum>>();
  }

  void react(LightsOff const &) override {
    Lights<inum>::template transit<lights::Off<inum>>();
  }
};

template <int inum>
class Blinking : public Lights<inum> {
  using base = Lights<inum>;

  void entry() override {
    auto &controller = Controller::GetInstance();
    controller.SetLightsStatus(inum, DE_ON, false);
    controller.SetLightsStatus(inum, DE_OFF, false);
    controller.SetLightsStatus(inum, DE_BLINK, true);
    base::cancel_ = false;
    base::blink_task_future_ = std::async(std::launch::async, [] {
      while (true) {
        spdlog::info("Lights<{}> on", inum);
        std::this_thread::sleep_for(kBlinkPeriod);
        if (base::cancel_.load()) break;
        spdlog::info("Lights<{}> off", inum);
        std::this_thread::sleep_for(kBlinkPeriod);
        if (base::cancel_.load()) break;
      }
    });
    spdlog::info("Lights<{}> blink", inum);
  }

  void Cancel() {
    base::cancel_ = true;
    if (base::blink_task_future_.valid()) {
      try {
        base::blink_task_future_.get();
      } catch (std::exception const &e) {
        spdlog::error("Lights<{}> error while cancelling blink: {}", inum,
                      e.what());
      }
    }
  }

  void react(LightsOn const &) override {
    Cancel();
    Lights<inum>::template transit<lights::On<inum>>();
  }

  void react(LightsOff const &) override {
    Cancel();
    Lights<inum>::template transit<lights::Off<inum>>();
  }
};

template <int inum>
class Off : public Lights<inum> {
  using base = Lights<inum>;

  void entry() override {
    auto &controller = Controller::GetInstance();
    controller.SetLightsStatus(inum, DE_ON, false);
    controller.SetLightsStatus(inum, DE_OFF, true);
    controller.SetLightsStatus(inum, DE_BLINK, false);
    spdlog::info("Lights<{}> off", inum);
  }

  void react(LightsOn const &) override {
    Lights<inum>::template transit<lights::On<inum>>();
  }

  void react(LightsBlink const &) override {
    Lights<inum>::template transit<lights::Blinking<inum>>();
  }
};
}  // namespace lights

FSM_INITIAL_STATE(Lights<0>, lights::Off<0>)
FSM_INITIAL_STATE(Lights<1>, lights::Off<1>)
