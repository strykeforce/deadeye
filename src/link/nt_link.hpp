#pragma once
#include <ntcore.h>
#include <spdlog/spdlog.h>
#include "link.hpp"

namespace deadeye {
class NetworkTablesLink : public Link {
 public:
  void init() override;
  bool poll() override;

 private:
  NT_Inst inst;
  static spdlog::level::level_enum nt2spdlogLevel(const nt::LogMessage& msg);
};
}  // namespace deadeye
