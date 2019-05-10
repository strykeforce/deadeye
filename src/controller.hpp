#pragma once
#include <ntcore.h>
#include <spdlog/spdlog.h>

namespace deadeye {
class Controller {
 public:
  static Controller& GetInstance() {
    static Controller instance;
    return instance;
  }

  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;
  ~Controller();
  int Run();
  void EnableLights(bool enabled);

 private:
  Controller();
  void StartNetworkTables();
  void SetNetworkTablesDefaults();
  void StartPoller();
  static spdlog::level::level_enum Nt2spdlogLevel(const nt::LogMessage& msg);

  NT_Inst inst;
  NT_EntryListenerPoller poller;
  NT_EntryListener entry_listener;
};
}  // namespace deadeye
