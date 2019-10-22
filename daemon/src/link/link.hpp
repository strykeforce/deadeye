#pragma once
#include <string>

#include "link/target_data.hpp"

namespace deadeye {

struct LinkConfig;

class Link {
 public:
  Link(int inum);
  ~Link();
  Link(const Link&) = delete;
  Link& operator=(const Link&) = delete;

  void Send(TargetData* const td);  // Caller retains ownership

 private:
  bool enabled_;
  std::string id_;
  int fd_;
  LinkConfig GetConfig();
};

}  // namespace deadeye
