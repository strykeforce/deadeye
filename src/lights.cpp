#include <spdlog/spdlog.h>
#include <tinyfsm.hpp>
#include "controller.hpp"
#include "lights.hpp"

using namespace deadeye;

// ---------------------------------------------------------------------------
// Lights states
//
namespace lights {
template <int inum>
class Off;

template <int inum>
class On : public Lights<inum> {
  void entry() override {
    Controller::GetInstance().SetLightsStatus(inum, true);
    spdlog::info("Lights<{}> on", inum);
  }

  void react(LightsOff const &) override {
    Lights<inum>::template transit<lights::Off<inum>>();
  }
};

template <int inum>
class Off : public Lights<inum> {
  void entry() override {
    Controller::GetInstance().SetLightsStatus(inum, false);
    spdlog::info("Lights<{}> off", inum);
  }
  void react(LightsOn const &) override {
    Lights<inum>::template transit<lights::On<inum>>();
  }
};
}  // namespace lights

FSM_INITIAL_STATE(Lights<0>, lights::Off<0>);
FSM_INITIAL_STATE(Lights<1>, lights::Off<1>);
