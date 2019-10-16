#include "stream_config.hpp"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <sstream>

using namespace deadeye;
using json = nlohmann::json;

char const* StreamConfig::kSerialKey{"sn"};
char const* StreamConfig::kUrlKey{"url"};
char const* StreamConfig::kViewKey{"view"};
char const* StreamConfig::kContourKey{"contour"};

namespace {
std::string stream_url(int inum);
}

StreamConfig::StreamConfig() {}

StreamConfig::StreamConfig(int inum)
    : sn(0), url(stream_url(inum)), view(View::NONE), contour(Contour::NONE) {}

StreamConfig::StreamConfig(int sn, std::string url, View view, Contour contour)
    : sn(sn), url(url), view(view), contour(contour) {}

StreamConfig::StreamConfig(std::shared_ptr<nt::Value> value) {
  auto j = json::parse(value->GetString().str());
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

std::ostream& operator<<(std::ostream& os, StreamConfig const& sc) {
  os << "StreamConfig{" << sc.sn << ", " << sc.url << "}";
  return os;
}

// ---------------------------------------------------------------------------
// address support
//

namespace {
const uint32_t nm08 = htonl(0xFF000000);
const uint32_t nm12 = htonl(0xFFF00000);
const uint32_t nm16 = htonl(0xFFFF0000);
const uint32_t n010 = htonl(0x0A000000) & nm08;  // 10.0.0.0-10.255.255.255
const uint32_t n172 = htonl(0xAC100000) & nm12;  // 172.16.0.0-172.31.255.255
const uint32_t n192 = htonl(0xC0A80000) & nm16;  // 192.168.0.0-192.168.255.255

bool is_private(struct sockaddr* addr) {
  if (addr == nullptr) return false;
  uint32_t ip = ((sockaddr_in*)addr)->sin_addr.s_addr;
  return (n010 == (ip & nm08)) || (n172 == (ip & nm12)) ||
         (n192 == (ip & nm16));
}

std::string first_rfc1918() {
  struct ifaddrs* ifaddr;
  std::string addr = "error";

  if (getifaddrs(&ifaddr) == -1) {
    spdlog::critical("Error getting interface address for stream: {}",
                     strerror(errno));
    return addr;
  }

  for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == nullptr) continue;
    if (ifa->ifa_netmask == nullptr) continue;
    if (((sockaddr_in*)(ifa->ifa_addr))->sin_family != AF_INET) continue;

    if (is_private(ifa->ifa_addr)) {
      char ip_str[INET_ADDRSTRLEN];
      in_addr ip = ((sockaddr_in*)(ifa->ifa_addr))->sin_addr;
      inet_ntop(AF_INET, &ip, ip_str, INET_ADDRSTRLEN);
      addr = ip_str;
      break;
    }
  }

  freeifaddrs(ifaddr);
  return addr;
}

std::string stream_url(int inum) {
  std::string url = "http://" + first_rfc1918() + ":" +
                    std::to_string(5800 + inum) + "/stream.mjpg";
  return url;
}

}  // namespace