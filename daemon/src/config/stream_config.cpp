// Copyright (c) 2022 Stryke Force FRC 2767

#include "stream_config.h"

#include <fmt/core.h>

#include <memory>
#include <utility>

#include "config.h"

using ::deadeye::StreamConfig;
using json = nlohmann::json;

namespace {
std::string stream_url(int inum);
}

StreamConfig::StreamConfig()
    : sn(0), view(View::none), contour(Contour::none) {}

StreamConfig::StreamConfig(int inum)
    : sn(0), url(stream_url(inum)), view(View::none), contour(Contour::none) {}

StreamConfig::StreamConfig(int sn, std::string url, View view, Contour contour)
    : sn(sn), url(std::move(url)), view(view), contour(contour) {}

StreamConfig::StreamConfig(const std::shared_ptr<nt::Value>& value) {
  assert(value);
  auto j = json::parse(value->GetString());
  j.at(kSerialKey).get_to(sn);
  j.at(kUrlKey).get_to(url);
  j.at(kViewKey).get_to(view);
  j.at(kContourKey).get_to(contour);
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

std::ostream& operator<<(std::ostream& os, const StreamConfig& sc) {
  os << fmt::format("StreamConfig<{}, {}>", sc.sn, sc.url);
  return os;
}

// ---------------------------------------------------------------------------
// address support
//

namespace {

std::string stream_url(int inum) {
  char* env_stream_host = std::getenv("DEADEYE_STREAM_HOST");
  const char* stream_host = env_stream_host != nullptr ? env_stream_host : DEADEYE_STREAM_ADDRESS;
  return fmt::format("http://{}:{}/stream.mjpg?s=0", stream_host,
                     5805 + inum);
}

}  // namespace
