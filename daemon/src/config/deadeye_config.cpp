#include "deadeye_config.h"

#include <fmt/core.h>

std::string deadeye::CameraControlTablePath(int inum) {
  return fmt::format("{}/{}", DE_CONTROL_TABLE, inum);
}

std::string deadeye::LightsControlTablePath(int inum) {
  return fmt::format("{}{}", deadeye::CameraControlTablePath(inum), DE_LIGHTS);
}

std::string deadeye::CameraConfigEntryPath(int inum) {
  return fmt::format("{}/{}{}", DE_CONFIG_TABLE, inum, DE_CONFIG);
}

std::string deadeye::StreamConfigEntryPath(int inum) {
  return fmt::format("{}/{}{}", DE_CONFIG_TABLE, inum, DE_STREAM);
}
