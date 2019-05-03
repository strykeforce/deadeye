#include <spdlog/spdlog.h>
#include <cstdlib>
#include <iostream>
#include "controller.hpp"

namespace log = spdlog;

int main(int argc, char** argv) {
  log::info("Deadeye starting");
  if (std::getenv("DEADEYE_NT_SERVER"))
    log::info("Starting as NetworkTables server");

  Controller controller;
  controller.run();

  return EXIT_SUCCESS;
}
