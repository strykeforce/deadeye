#include "deadeye_config.h"

#include <fmt/core.h>

std::string deadeye::CameraControlTablePath(int inum) {
  return fmt::format("{}/{}", DE_CONTROL_TABLE, inum);
}

std::string deadeye::LightsControlTablePath(int inum) {
  return fmt::format("{}{}", deadeye::CameraControlTablePath(inum), DE_LIGHTS);
}

std::string deadeye::CaptureConfigEntryPath(int inum) {
  return fmt::format("{}/{}{}", DE_CONFIG_TABLE, inum, DE_CAPTURE);
}

std::string deadeye::PipelineConfigEntryPath(int inum) {
  return fmt::format("{}/{}{}", DE_CONFIG_TABLE, inum, DE_PIPELINE);
}

std::string deadeye::StreamConfigEntryPath(int inum) {
  return fmt::format("{}/{}{}", DE_CONFIG_TABLE, inum, DE_STREAM);
}

std::string deadeye::InfoEntryPath(int inum) {
  return fmt::format("{}/{}{}", DE_CONFIG_TABLE, inum, DE_INFO);
}
