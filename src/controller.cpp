#include <spdlog/spdlog.h>
#include "controller.hpp"

namespace log = spdlog;

void Controller::run() { log::info("Controller {}!", "Running"); }
