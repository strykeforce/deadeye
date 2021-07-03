#include <chrono>
#include <gpiod.hpp>
#include <thread>

#include "log.h"

int main() {
  using namespace std::chrono_literals;
  using namespace gpiod;

  deadeye::log::Configure("shutdown");

  chip c{"gpiochip0", chip::OPEN_BY_NAME};
  auto line = c.get_line(216);
  line_request lr{"shutdownd", line_request::DIRECTION_INPUT, 0};
  line.request(lr, 0);

  int count = 0;
  while (true) {
    int input = line.get_value();
    spdlog::debug("input={}", input);
    if (input)
      count++;
    else
      count = 0;

    if (count == 3) {
      spdlog::warn("Shutdown button triggered shutdown");
      return EXIT_SUCCESS;
    }

    std::this_thread::sleep_for(1s);
  }

  return EXIT_FAILURE;
}
