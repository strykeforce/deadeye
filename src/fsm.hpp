#pragma once

#include <tinyfsm.hpp>

#include "camera.hpp"
#include "lights.hpp"

namespace deadeye {
using fsm = tinyfsm::FsmList<Camera<0>, Camera<1>, Lights<0>, Lights<1>>;

template <typename E>
void SendEvent(E const& event) {
  fsm::template dispatch<E>(event);
}
}  // namespace deadeye
