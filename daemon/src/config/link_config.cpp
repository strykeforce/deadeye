#include "link_config.h"

using namespace deadeye;
using json = nlohmann::json;

const char* LinkConfig::kAddressKey{"address"};
const char* LinkConfig::kPortKey{"port"};
const char* LinkConfig::kEnabledKey{"enabled"};

LinkConfig::LinkConfig() {}

LinkConfig::LinkConfig(std::string address, int port, bool enabled)
    : address(address), port(port), enabled(enabled) {}

LinkConfig::LinkConfig(std::shared_ptr<nt::Value> value) {
  auto j = json::parse(value->GetString().str());
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
