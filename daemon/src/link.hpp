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
