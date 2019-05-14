#include <spdlog/spdlog.h>
#include <future>
#include <tinyfsm.hpp>
#include "camera.hpp"
#include "controller.hpp"
#include "fsm.hpp"
#include "lights.hpp"
#include "pipeline.hpp"

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

  void entry() override {
    base::pipeline_future_ = std::async(std::launch::async, [] {
      try {
        base::pipeline_.Run();
      } catch (...) {
        // queue CameraOff via Controller
        Controller::GetInstance().SetCameraStatus(inum, false);
        std::rethrow_exception(std::current_exception());
      }
    });
    spdlog::info("Camera<{}> on", inum);
  }

  void react(CameraOff const &) override {
    Lights<inum>::dispatch(LightsOff());
    base::pipeline_.Quit();

    // future will not be valid at start-up since pipeline task not started yet
    if (base::pipeline_future_.valid()) {
      try {
        base::pipeline_future_.get();
      } catch (std::exception const &e) {
        base::error_ = e.what();
        base::template transit<camera::Error<inum>>();
        return;
      }
    }
    base::template transit<camera::Off<inum>>();
  }
};

// ---------------------------------------------------------------------------
// Camera State: Off
//
template <int inum>
class Off : public Camera<inum> {
  using base = Camera<inum>;

  void entry() override {
    // make sure NT reflects off state
    Controller::GetInstance().SetCameraStatus(inum, false);
    spdlog::info("Camera<{}> off", inum);
  }

  void react(CameraOn const &) override {
    Lights<inum>::dispatch(LightsOn());
    base::template transit<camera::On<inum>>();
  }
};

// ---------------------------------------------------------------------------
// Camera State: Error
//
template <int inum>
class Error : public Camera<inum> {
  using base = Camera<inum>;

  void entry() override {
    // make sure NT reflects error state
    Controller::GetInstance().SetCameraError(inum, true);
    spdlog::error("Camera<{}> error: {}", inum, base::error_);
  }

  void react(CameraOn const &) override {
    // make sure NT reflects off state
    Controller::GetInstance().SetCameraStatus(inum, false);
    spdlog::warn("Camera<{}> attempting to restart camera in error state: {}",
                 inum, base::error_);
  }
};

}  // namespace camera
}  // namespace deadeye

FSM_INITIAL_STATE(deadeye::Camera<0>, deadeye::camera::Off<0>)
FSM_INITIAL_STATE(deadeye::Camera<1>, deadeye::camera::Off<1>)
