#include "link.hpp"
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

using namespace deadeye;

Link::Link(/* args */) {
  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ == -1)
    spdlog::critical("Link send socket error: {}", strerror(errno));

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(CLIENT_PORT);
  inet_pton(AF_INET, CLIENT_ADDRESS, &addr.sin_addr);

  if (connect(fd_, (sockaddr*)&addr, sizeof(addr)) == -1)
    spdlog::critical("Link connect error: {}", strerror(errno));

  spdlog::info("Link connected to {}:{}", CLIENT_ADDRESS, CLIENT_PORT);
}

Link::~Link() {
  if (close(fd_) == -1) spdlog::error("Link close error: {}", strerror(errno));
  spdlog::info("Link closed socket");
}

void Link::Send() {
  int n;
  nlohmann::json j;
  j["id"] = "A1";
  j["sn"] = sn_++;
  j["valid"] = false;
  j["x"] = sn_;
  j["y"] = sn_;

  auto msg = j.dump().c_str();
  if ((n = send(fd_, msg, strlen(msg), 0)) == -1)
    spdlog::error("Link send error: {}", strerror(errno));
}
