#include "link.hpp"
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include "controller.hpp"

using namespace deadeye;

Link::Link(/* args */) {
  const char* client_address =
      Controller::GetInstance().GetClientAddress().c_str();
  int client_port = Controller::GetInstance().GetClientPort();

  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ == -1)
    spdlog::critical("Link send socket error: {}", strerror(errno));

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(client_port);
  inet_pton(AF_INET, client_address, &addr.sin_addr);

  if (connect(fd_, (sockaddr*)&addr, sizeof(addr)) == -1)
    spdlog::critical("Link connect error: {}", strerror(errno));

  spdlog::info("Link connected to {}:{}", client_address, client_port);
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
