#include <spdlog/spdlog.h>
#include <chrono>
#include <csignal>
#include <thread>
#include "controller.hpp"

using namespace deadeye;

int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::debug);
  spdlog::info("Deadeye starting");
  return Controller::GetInstance().Run();
}
