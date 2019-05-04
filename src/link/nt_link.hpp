#pragma once
#include <ntcore.h>
#include <spdlog/spdlog.h>
#include "link.hpp"

namespace deadeye {
class NetworkTablesLink : public Link {
 public:
  void Init() override;
  bool Poll() override;

 private:
  NT_Inst inst;
  NT_EntryListenerPoller poller;
  NT_EntryListener entry_listener;

  static spdlog::level::level_enum Nt2spdlogLevel(const nt::LogMessage& msg);
  void StartNetworkTables();
  void SetDefaults();
  void StartPoller();
};
}  // namespace deadeye
