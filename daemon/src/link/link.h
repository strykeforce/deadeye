#pragma once
#include <networktables/NetworkTableInstance.h>

#include <string>

#include "link/target_data.h"

namespace deadeye {

struct LinkConfig;

class Link {
 public:
  Link(int inum);
  ~Link();
  Link(const Link&) = delete;
  Link& operator=(const Link&) = delete;

  void Send(TargetData* const td) const;  // Caller retains ownership

  static void Init(nt::NetworkTableInstance& nti);

 private:
  bool enabled_;
  std::string id_;
  int fd_;
  LinkConfig GetConfig();
};

}  // namespace deadeye
