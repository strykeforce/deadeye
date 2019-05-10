#include <spdlog/spdlog.h>
#include <tinyfsm.hpp>
#include "camera.hpp"
#include "fsm.hpp"

using namespace deadeye;

// ---------------------------------------------------------------------------
// Camera states
//
namespace camera {
class Off;

class On : public Camera {
  void entry() override { spdlog::info("Camera On"); }

  void react(CameraOff const &) override {
    fsm::dispatch(LightsOff());
    transit<camera::Off>();
  }
};

class Off : public Camera {
  void entry() override { spdlog::info("Camera Off"); }

  void react(CameraOn const &) override {
    fsm::dispatch(LightsOn());
    transit<camera::On>();
  }
};
}  // namespace camera

FSM_INITIAL_STATE(Camera, camera::Off);
