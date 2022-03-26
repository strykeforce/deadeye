// Copyright (c) 2022 Stryke Force FRC 2767

#include "link.h"

#include <arpa/inet.h>
#include <fmt/core.h>
#include <networktables/NetworkTableInstance.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <unistd.h>

#include <nlohmann/json.hpp>

#include "config.h"
#include "config/link_config.h"
#include "controller.h"
#include "link/upright_target_data.h"

using ::deadeye::Link;
using ::deadeye::LinkConfig;

Link::Link(int inum) : id_(DEADEYE_UNIT + std::to_string(inum)) {
  LinkConfig link_config = GetConfig();
  enabled_ = link_config.enabled;
  spdlog::log(enabled_ ? spdlog::level::info : spdlog::level::warn,
              "Link<{}>: {}:{} {}", id_, link_config.address, link_config.port,
              enabled_ ? "[enabled]" : "[disabled]");

  if (!enabled_) return;

  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ == -1)
    spdlog::critical("Link<{}> send socket error: {}", id_, strerror(errno));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(link_config.port);
  inet_pton(AF_INET, link_config.address.c_str(), &addr.sin_addr);

  if (connect(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
    spdlog::critical("Link<{}> connect error: {}", id_, strerror(errno));

  spdlog::info("Link<{}> connecting to {}:{}", id_, link_config.address,
               link_config.port);
}

Link::~Link() {
  if (!enabled_) return;
  if (close(fd_) == -1) spdlog::error("Link close error: {}", strerror(errno));
  spdlog::info("Link<{}> closed socket", id_);
}

LinkConfig Link::GetConfig() {
  auto nti = nt::NetworkTableInstance(nt::GetDefaultInstance());
  auto js = nti.GetEntry(DE_LINK_ENTRY).GetString("[]");
  auto j = json::parse(js);
  return LinkConfig{j[0]};  // TODO(jhh): configure all links in list
}

void Link::Send(TargetData* const td) const {
  if (!enabled_) return;

  std::string msg = id_ + td->Dump();
  if (send(fd_, msg.data(), msg.size(), 0) == -1)
    spdlog::error("Link<{}> send error: {}", id_, strerror(errno));
}

void Link::Init(nt::NetworkTableInstance& nti) {
  auto entry = nti.GetEntry(DE_LINK_ENTRY);
  LinkConfig lc{DEADEYE_CLIENT_ADDRESS, DEADEYE_CLIENT_PORT, true};
  json j = {lc};
  entry.SetDefaultString(j.dump());
  entry.SetPersistent();
}
