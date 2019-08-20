#include "stream_config.hpp"

using namespace deadeye;
using json = nlohmann::json;

char const* StreamConfig::kSerialKey{"sn"};
char const* StreamConfig::kUrlKey{"url"};
char const* StreamConfig::kViewKey{"view"};
char const* StreamConfig::kContourKey{"contour"};

StreamConfig StreamConfig::New(std::shared_ptr<nt::Value> value) {
  auto j = json::parse(value->GetString().str());
  return j.get<StreamConfig>();
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const StreamConfig& sc) {
  j = json{
      {StreamConfig::kSerialKey, sc.sn},
      {StreamConfig::kUrlKey, sc.url},
      {StreamConfig::kViewKey, sc.view},
      {StreamConfig::kContourKey, sc.contour},
  };
}

void deadeye::from_json(const json& j, StreamConfig& sc) {
  j.at(StreamConfig::kSerialKey).get_to(sc.sn);
  j.at(StreamConfig::kUrlKey).get_to(sc.url);
  j.at(StreamConfig::kViewKey).get_to(sc.view);
  j.at(StreamConfig::kContourKey).get_to(sc.contour);
}

// ---------------------------------------------------------------------------
// iostream support
//

std::ostream& operator<<(std::ostream& os, StreamConfig const& sc) {
  os << "StreamConfig{" << sc.sn << ", " << sc.url << "}";
  return os;
}
