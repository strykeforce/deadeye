#pragma once
#include <networktables/NetworkTableInstance.h>

#include <string>

#include "link/target_data.h"

namespace deadeye {

struct LinkConfig;

class Link {
 public:
  explicit Link(int inum);
  ~Link();
  Link(const Link&) = delete;
  Link& operator=(const Link&) = delete;

  void Send(TargetData* td) const;  // Caller retains ownership

  static void Init(nt::NetworkTableInstance& nti);

 private:
  bool enabled_;
  std::string id_;
  int fd_;
  static LinkConfig GetConfig();
};

}  // namespace deadeye
