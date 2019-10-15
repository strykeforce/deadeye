#pragma once
#include <string>

namespace deadeye {

struct LinkConfig;

class Link {
 public:
  Link(int inum);
  ~Link();
  Link(const Link&) = delete;
  Link& operator=(const Link&) = delete;

  void Send();

 private:
  bool enabled_;
  std::string id_;
  int fd_;
  int sn_ = 0;
  LinkConfig GetConfig();
};

}  // namespace deadeye
