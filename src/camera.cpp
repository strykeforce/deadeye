#include <spdlog/spdlog.h>
#include <tinyfsm.hpp>
#include "camera.hpp"
#include "fsm.hpp"
#include "lights.hpp"

namespace deadeye {
namespace camera {
template <int inum>
class Off;  // forward declaration

// ---------------------------------------------------------------------------
// Camera State: On
//
template <int inum>
class On : public Camera<inum> {
  void entry() override { spdlog::info("Camera<{}> On", inum); }

  void react(CameraOff const &) override {
    Lights<inum>::dispatch(LightsOff());
    Camera<inum>::template transit<camera::Off<inum>>();
  }

  void react(ShutDown const &) override {
    Camera<inum>::template transit<camera::Off<inum>>();
  }
};

// ---------------------------------------------------------------------------
// Camera State: Off
//
template <int inum>
class Off : public Camera<inum> {
  void entry() override { spdlog::info("Camera<{}> Off", inum); }

  void react(CameraOn const &) override {
    Lights<inum>::dispatch(LightsOn());
    Camera<inum>::template transit<camera::On<inum>>();
  }
};
}  // namespace camera

}  // namespace deadeye

FSM_INITIAL_STATE(deadeye::Camera<0>, deadeye::camera::Off<0>);
FSM_INITIAL_STATE(deadeye::Camera<1>, deadeye::camera::Off<1>);
