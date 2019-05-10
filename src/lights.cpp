#include <spdlog/spdlog.h>
#include <tinyfsm.hpp>
#include "controller.hpp"
#include "lights.hpp"

using namespace deadeye;

// ---------------------------------------------------------------------------
// Lights states
//
namespace lights {
class Off;

class On : public Lights {
  void entry() override {
    Controller::GetInstance().EnableLights(true);
    spdlog::info("Lights On");
  }
  void react(LightsOff const &) override { transit<lights::Off>(); }
};

class Off : public Lights {
  void entry() override {
    Controller::GetInstance().EnableLights(false);
    spdlog::info("Lights Off");
  }
  void react(LightsOn const &) override { transit<lights::On>(); }
};
}  // namespace lights

FSM_INITIAL_STATE(Lights, lights::Off);
