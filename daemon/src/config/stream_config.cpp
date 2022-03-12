#include "stream_config.h"

#include <arpa/inet.h>
#include <fmt/core.h>
#include <ifaddrs.h>
#include <spdlog/spdlog.h>
#include <cstdio>

#include <sstream>
#include <utility>
#include "config.h"

using namespace deadeye;
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
const uint32_t nm08 = htonl(0xFF000000); // NOLINT
const uint32_t nm12 = htonl(0xFFF00000); // NOLINT
const uint32_t nm16 = htonl(0xFFFF0000); // NOLINT

// 10.0.0.0-10.255.255.255
const uint32_t n010 = htonl(0x0A000000) & nm08; // NOLINT
// 172.16.0.0-172.31.255.255
const uint32_t n172 = htonl(0xAC100000) & nm12; // NOLINT
// 192.168.0.0-192.168.255.255
const uint32_t n192 = htonl(0xC0A80000) & nm16; // NOLINT

std::string stream_url(int inum) {
  if (std::getenv("DEADEYE_DOCKER")) {
    return fmt::format("/stream/{}/?s=0", inum);
  }
  return fmt::format("http://{}:{}/stream.mjpg?s=0", DEADEYE_STREAM_ADDRESS,
                     5805 + inum);
}

}  // namespace
