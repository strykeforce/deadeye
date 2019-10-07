#pragma once

namespace deadeye {

class Link {
 public:
  Link(/* args */);
  ~Link();
  Link(const Link&) = delete;
  Link& operator=(const Link&) = delete;

  void Send();

 private:
  int fd_;
  int sn_ = 0;
};

}  // namespace deadeye

#define LINK_STRINGIFY(s) LINK_STR(s)
#define LINK_STR(s) #s

#define CLIENT_PORT DEADEYE_CLIENT_PORT
#define CLIENT_ADDRESS LINK_STRINGIFY(DEADEYE_CLIENT_ADDRESS)
