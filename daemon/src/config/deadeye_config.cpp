#include "deadeye_config.hpp"

std::string deadeye::CameraControlTablePath(int inum) {
  std::string control_table{DE_CONTROL_TABLE};
  control_table += "/";
  control_table += std::to_string(inum);
  return control_table;
}

std::string deadeye::LightsControlTablePath(int inum) {
  std::string control_table = deadeye::CameraControlTablePath(inum);
  control_table += std::string(DE_LIGHTS);
  return control_table;
}

std::string deadeye::CameraConfigEntryPath(int inum) {
  std::string config_entry{DE_CONFIG_TABLE};
  config_entry += "/";
  config_entry += std::to_string(inum);
  config_entry += std::string(DE_CONFIG);
  return config_entry;
}

std::string deadeye::StreamConfigEntryPath(int inum) {
  std::string config_entry{DE_CONFIG_TABLE};
  config_entry += "/";
  config_entry += std::to_string(inum);
  config_entry += std::string(DE_STREAM);
  return config_entry;
}
