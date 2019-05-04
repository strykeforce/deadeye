#include <spdlog/spdlog.h>
#include <chrono>
#include <thread>
#include "controller.hpp"
#include "link/nt_link.hpp"

using namespace deadeye;
namespace log = spdlog;

int main(int argc, char** argv) {
  log::set_level(log::level::debug);
  log::info("Deadeye starting");

  Controller controller;
  controller.run();

  NetworkTablesLink link;
  link.Init();

  while (1) link.Poll();

  while (1) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  return EXIT_SUCCESS;
}
