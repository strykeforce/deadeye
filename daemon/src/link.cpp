#include "link.hpp"
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include "controller.hpp"
#include "link_config.hpp"

using namespace deadeye;
Link::Link(int inum) : id_(DEADEYE_UNIT + std::to_string(inum)) {
  LinkConfig link_config = Controller::GetInstance().GetLinkConfig();

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
  if (close(fd_) == -1) spdlog::error("Link close error: {}", strerror(errno));
  spdlog::info("Link closed socket");
}

void Link::Send() {
  int n;
  nlohmann::json j;
  j["id"] = id_;
  j["sn"] = sn_++;
  j["valid"] = false;
  j["x"] = sn_;
  j["y"] = sn_;

  auto msg = j.dump().c_str();
  if ((n = send(fd_, msg, strlen(msg), 0)) == -1)
    spdlog::error("Link send error: {}", strerror(errno));
}
