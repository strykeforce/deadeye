#include <sys/reboot.h>
#include <systemd/sd-bus.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <gpiod.hpp>
#include <thread>

#define DESTINATION "org.freedesktop.login1"
#define PATH "/org/freedesktop/login1"
#define INTERFACE "org.freedesktop.login1.Manager"
#define MEMBER "PowerOff"

#include "log.h"

int do_normal_shutdown();
int do_fast_shutdown();

int main() {
  using namespace std::chrono_literals;
  using namespace gpiod;

  deadeye::log::Configure("shutdown");

  bool fast_shutdown = std::getenv("DEADEYE_FAST_SHUTDOWN") != nullptr;
  spdlog::info("Fast shutdown enabled: {}", fast_shutdown);

  spdlog::info("Configuring GPIO line.");
  chip c{"gpiochip0", chip::OPEN_BY_NAME};
  auto line = c.get_line(216);
  line_request lr{"shutdownd", line_request::DIRECTION_INPUT, 0};
  line.request(lr, 0);

  int count = 0;
  spdlog::info("Waiting for shutdown button press...");
  while (true) {
    int input = line.get_value();
    if (input)
      count++;
    else
      count = 0;

    if (count == 3) {
      spdlog::info("Shutdown button triggered shutdown");
      if (fast_shutdown) return do_fast_shutdown();
      return do_normal_shutdown();
    }

    std::this_thread::sleep_for(1s);
  }

  return EXIT_FAILURE;
}

int do_normal_shutdown() {
  spdlog::info("Performing normal shutdown");
  sd_bus* bus = NULL;
  int r = sd_bus_open_system(&bus);
  if (r != 0) {
    spdlog::error("Error getting default system bus: {}", std::strerror(r));
    return EXIT_FAILURE;
  }

  sd_bus_error error = SD_BUS_ERROR_NULL;
  spdlog::info("Got default system bus");
  spdlog::info("Calling {}", MEMBER);

  r = sd_bus_call_method(bus, DESTINATION, PATH, INTERFACE, MEMBER, &error,
                         NULL, "b", 0);

  return EXIT_SUCCESS;
}

int do_fast_shutdown() {
  spdlog::info("Performing fast shutdown");
  sync();
  reboot(RB_POWER_OFF);
  return EXIT_SUCCESS;
}
