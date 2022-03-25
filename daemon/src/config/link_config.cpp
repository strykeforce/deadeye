// Copyright (c) 2022 Stryke Force FRC 2767

#include "link_config.h"

#include <utility>

using ::deadeye::LinkConfig;
using json = nlohmann::json;

LinkConfig::LinkConfig(std::string address, int port, bool enabled)
    : address(std::move(address)), port(port), enabled(enabled) {}

LinkConfig::LinkConfig(const nlohmann::json& j) {
  j.at(LinkConfig::kAddressKey).get_to(address);
  j.at(LinkConfig::kPortKey).get_to(port);
  j.at(LinkConfig::kEnabledKey).get_to(enabled);
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const LinkConfig& l) {
  j = json{{LinkConfig::kAddressKey, l.address},
           {LinkConfig::kPortKey, l.port},
           {LinkConfig::kEnabledKey, l.enabled}};
}

void deadeye::from_json(const json& j, LinkConfig& l) {
  j.at(LinkConfig::kAddressKey).get_to(l.address);
  j.at(LinkConfig::kPortKey).get_to(l.port);
  j.at(LinkConfig::kEnabledKey).get_to(l.enabled);
}
