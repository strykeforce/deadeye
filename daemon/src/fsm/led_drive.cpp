#include <fsm/led_drive.h>

#include <spdlog/spdlog.h>

#include "config/deadeye_config.h"

using namespace deadeye;

#ifdef __aarch64__
using namespace gpiod;

LedDrive::LedDrive(int inum) {
  assert(inum >= 0 && inum < 5);

  chip c{"gpiochip0", chip::OPEN_BY_NAME};
  line_ = c.get_line(inum + 16);

  line_request lr{"deadeyed", line_request::DIRECTION_OUTPUT, 0};
  line_.request(lr, 1);
}

void LedDrive::On() { line_.set_value(0); }

void LedDrive::Off() { line_.set_value(1); }
#else
LedDrive::LedDrive(int inum) {}
void LedDrive::On() {}
void LedDrive::Off() {}
#endif
