#include "link.hpp"

#include <arpa/inet.h>
#include <fmt/core.h>
#include <networktables/NetworkTableInstance.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

#include "config/deadeye_config.hpp"
#include "config/link_config.hpp"
#include "controller.hpp"
#include "link/center_target_data.hpp"

using namespace deadeye;
Link::Link(int inum) : id_(DEADEYE_UNIT + std::to_string(inum)) {
  LinkConfig link_config = GetConfig();
  enabled_ = link_config.enabled;
  if (!enabled_) {
    spdlog::warn("Link is disabled");
    return;
  };

  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ == -1)
    spdlog::critical("Link send socket error: {}", strerror(errno));

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(link_config.port);
  inet_pton(AF_INET, link_config.address.c_str(), &addr.sin_addr);

  if (connect(fd_, (sockaddr*)&addr, sizeof(addr)) == -1)
    spdlog::critical("Link connect error: {}", strerror(errno));

  spdlog::info("Link {} connected to {}:{}", id_, link_config.address,
               link_config.port);
}

Link::~Link() {
  if (!enabled_) return;
  if (close(fd_) == -1) spdlog::error("Link close error: {}", strerror(errno));
  spdlog::info("Link closed socket");
}

LinkConfig Link::GetConfig() {
  auto nti = nt::NetworkTableInstance(nt::GetDefaultInstance());
  return LinkConfig{nti.GetEntry(DE_LINK_CONFIG_ENTRY).GetValue()};
}

void Link::Send(TargetData* const td) {
  if (!enabled_) return;

  std::string msg = id_ + td->Dump();
  if (send(fd_, msg.data(), msg.size(), 0) == -1)
    spdlog::error("Link send error: {}", strerror(errno));
}
