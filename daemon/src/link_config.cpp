#include "link_config.hpp"

using namespace deadeye;
using json = nlohmann::json;

char const* LinkConfig::kAddressKey{"address"};
char const* LinkConfig::kPortKey{"port"};

LinkConfig::LinkConfig() {}

LinkConfig::LinkConfig(std::string address, int port)
    : address(address), port(port) {}

LinkConfig::LinkConfig(std::shared_ptr<nt::Value> value) {
  auto j = json::parse(value->GetString().str());
  j.at(LinkConfig::kAddressKey).get_to(address);
  j.at(LinkConfig::kPortKey).get_to(port);
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const LinkConfig& l) {
  j = json{{LinkConfig::kAddressKey, l.address},
           {LinkConfig::kPortKey, l.port}};
}

void deadeye::from_json(const json& j, LinkConfig& l) {
  j.at(LinkConfig::kAddressKey).get_to(l.address);
  j.at(LinkConfig::kPortKey).get_to(l.port);
}
