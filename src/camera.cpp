#include <spdlog/spdlog.h>
#include <tinyfsm.hpp>
#include "camera.hpp"
#include "fsm.hpp"
#include "lights.hpp"

using namespace deadeye;

// ---------------------------------------------------------------------------
// Camera states
//
namespace camera {
template <int inum>
class Off;

template <int inum>
class On : public Camera<inum> {
  using base = Camera<inum>;

  void entry() override { spdlog::info("Camera<{}> On", inum); }

  void react(CameraOff const &) override {
    fsm::dispatch(LightsOff());
    base::template transit<camera::Off<inum>>();
  }
};

template <int inum>
class Off : public Camera<inum> {
  using base = Camera<inum>;

  void entry() override { spdlog::info("Camera<{}> Off", inum); }

  void react(CameraOn const &) override {
    fsm::dispatch(LightsOn());
    Camera<inum>::template transit<camera::On<inum>>();
  }
};
}  // namespace camera

FSM_INITIAL_STATE(Camera<0>, camera::Off<0>);
FSM_INITIAL_STATE(Camera<1>, camera::Off<1>);
