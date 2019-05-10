#pragma once

#include <tinyfsm.hpp>
#include "camera.hpp"
#include "lights.hpp"

namespace deadeye {
using fsm = tinyfsm::FsmList<Camera, Lights>;

template <typename E>
void SendEvent(E const& event) {
  fsm::template dispatch<E>(event);
}
}  // namespace deadeye
