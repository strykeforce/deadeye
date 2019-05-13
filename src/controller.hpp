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
  void SetCameraStatus(int inum, bool enabled);
  void SetCameraError(int inum, bool error);
  void SetLightsStatus(int inum, bool enabled);

 private:
  Controller();
  void StartNetworkTables();
  void SetNetworkTablesDefaults();
  void StartPoller();
  static spdlog::level::level_enum Nt2spdlogLevel(const nt::LogMessage& msg);

  NT_Inst inst_;
  NT_EntryListenerPoller poller_;
  NT_EntryListener entry_listener_;
};
}  // namespace deadeye
