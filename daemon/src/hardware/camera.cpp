#include "camera.h"

#include <future>

#include "controller.h"
#include "lights.h"

using namespace std::chrono_literals;

namespace deadeye {
namespace camera {

// forward declarations
template <int inum>
class Off;
template <int inum>
class Error;

// ---------------------------------------------------------------------------
// Camera State: On
//
template <int inum>
class On : public Camera<inum> {
  using base = Camera<inum>;

  void entry() final {
    base::SetStatus(DE_ON, true);
    base::has_error_ = false;

    base::pipeline_future_ = std::async(std::launch::async, [] {
      try {
        base::pipeline_runner_.Run();
      } catch (...) {
        base::has_error_ = true;
        std::rethrow_exception(std::current_exception());
      }
    });
    spdlog::info("Camera<{}{}> on", DEADEYE_UNIT, inum);
  }

  void react(CameraOff const &) final {
    Lights<inum>::dispatch(LightsOff());
    base::pipeline_runner_.Stop();

    // future will not be valid at start-up since pipeline task not started yet
    if (base::pipeline_future_.valid()) {
      try {
        base::pipeline_future_.get();
      } catch (std::exception const &e) {
        base::error_ = e.what();
        base::has_error_ = false;  // don't retrigger in controller
        base::template transit<camera::Error<inum>>();
        return;
      }
    }
    base::template transit<camera::Off<inum>>();
  }

  void exit() final { base::SetStatus(DE_ON, false); }
};

// ---------------------------------------------------------------------------
// Camera State: Off
//
template <int inum>
class Off : public Camera<inum> {
  using base = Camera<inum>;

  void entry() final {
    base::SetStatus(DE_OFF, true);
    spdlog::info("Camera<{}{}> off", DEADEYE_UNIT, inum);
  }

  void react(CameraOn const &) final {
    Lights<inum>::dispatch(LightsOn());
    base::template transit<camera::On<inum>>();
  }

  void exit() final { base::SetStatus(DE_OFF, false); }
};

// ---------------------------------------------------------------------------
// Camera State: Error
//
template <int inum>
class Error : public Camera<inum> {
  using base = Camera<inum>;

  void entry() final {
    base::SetStatus(DE_ERROR, true);
    Lights<inum>::dispatch(LightsBlink());
    spdlog::error("Camera<{}{}> error: {}", DEADEYE_UNIT, inum, base::error_);
  }

  void react(CameraOn const &) final {
    base::SetStatus(DE_ON, false);
    spdlog::warn("Camera<{}{}> attempting to turn on camera in error state: {}",
                 DEADEYE_UNIT, inum, base::error_);
  }

  void react(CameraOff const &) final {
    base::SetStatus(DE_OFF, false);
    spdlog::warn(
        "Camera<{}{}> attempting to turn off camera in error state: {}",
        DEADEYE_UNIT, inum, base::error_);
  }

  void exit() final { base::SetStatus(DE_ERROR, false); }
};

}  // namespace camera
}  // namespace deadeye

FSM_INITIAL_STATE(deadeye::Camera<0>, deadeye::camera::Off<0>)
FSM_INITIAL_STATE(deadeye::Camera<1>, deadeye::camera::Off<1>)
FSM_INITIAL_STATE(deadeye::Camera<2>, deadeye::camera::Off<2>)
FSM_INITIAL_STATE(deadeye::Camera<3>, deadeye::camera::Off<3>)
FSM_INITIAL_STATE(deadeye::Camera<4>, deadeye::camera::Off<4>)
