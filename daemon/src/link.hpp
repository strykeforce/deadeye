#pragma once
#include <string>

namespace deadeye {

class Link {
 public:
  Link(int inum);
  ~Link();
  Link(const Link&) = delete;
  Link& operator=(const Link&) = delete;

  void Send();

 private:
  std::string id_;
  int fd_;
  int sn_ = 0;
};

}  // namespace deadeye
