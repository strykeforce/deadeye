#include <spdlog/spdlog.h>
#include <chrono>
#include <csignal>
#include <thread>
#include "controller.hpp"

using namespace deadeye;
namespace log = spdlog;

int main(int argc, char** argv) {
  log::set_level(log::level::debug);
  log::info("Deadeye starting");

  Controller controller;
  return controller.Run();
}
