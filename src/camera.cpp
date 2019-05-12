#include <spdlog/spdlog.h>
#include <thread>
#include <tinyfsm.hpp>
#include "camera.hpp"
#include "fsm.hpp"
#include "lights.hpp"
#include "pipeline.hpp"

using namespace std::chrono_literals;

namespace deadeye {
namespace camera {
template <int inum>
class Off;  // forward declaration

// ---------------------------------------------------------------------------
// Camera State: On
//
template <int inum>
class On : public Camera<inum> {
  using base = Camera<inum>;

  void entry() override {
    base::pipeline_thread_ =
        std::make_unique<std::thread>([] { base::pipeline_.Run(); });
    spdlog::info("{} on", static_cast<base &>(*this));
  }

  void react(CameraOff const &) override {
    Lights<inum>::dispatch(LightsOff());
    base::template transit<camera::Off<inum>>();
  }

  void react(ShutDown const &) override {
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
    base::pipeline_.Quit();
    if (base::pipeline_thread_ && base::pipeline_thread_->joinable()) {
      base::pipeline_thread_->join();
    }
    spdlog::info("{} off", static_cast<base &>(*this));
  }

  void react(CameraOn const &) override {
    Lights<inum>::dispatch(LightsOn());
    base::template transit<camera::On<inum>>();
  }
};
}  // namespace camera

}  // namespace deadeye

FSM_INITIAL_STATE(deadeye::Camera<0>, deadeye::camera::Off<0>);
FSM_INITIAL_STATE(deadeye::Camera<1>, deadeye::camera::Off<1>);
