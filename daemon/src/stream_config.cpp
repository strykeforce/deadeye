#include "stream_config.hpp"

using namespace deadeye;
using json = nlohmann::json;

char const* StreamConfig::kSerialKey{"sn"};
char const* StreamConfig::kUrlKey{"url"};

StreamConfig StreamConfig::New(std::shared_ptr<nt::Value> value) {
  auto j = json::parse(value->GetString().str());
  return j.get<StreamConfig>();
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const StreamConfig& sc) {
  j = json{{StreamConfig::kSerialKey, sc.sn}, {StreamConfig::kUrlKey, sc.url}};
}

void deadeye::from_json(const json& j, StreamConfig& sc) {
  j.at(StreamConfig::kSerialKey).get_to(sc.sn);
  j.at(StreamConfig::kUrlKey).get_to(sc.url);
}

// ---------------------------------------------------------------------------
// iostream support
//

std::ostream& operator<<(std::ostream& os, StreamConfig const& sc) {
  os << "StreamConfig{" << sc.sn << ", " << sc.url << "}";
  return os;
}
