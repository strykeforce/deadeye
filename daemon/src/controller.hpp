#pragma once

#include <ntcore.h>
#include <spdlog/spdlog.h>
#include "defs.hpp"
#include "link_config.hpp"

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
  void Run();
  void ShutDown();
  void SetCameraStatus(int inum, char const* name, bool state);
  void SetLightsStatus(int inum, char const* name, bool state);
  LinkConfig GetLinkConfig();

 private:
  Controller();
  void StartNetworkTables();
  void InitializeNetworkTableEntries();
  void InitializeCameraConfig();
  void StartPoller();
  static spdlog::level::level_enum Nt2spdlogLevel(const nt::LogMessage& msg);

  NT_Inst inst_;
  NT_EntryListenerPoller poller_;
  NT_EntryListener entry_listener_;
};
}  // namespace deadeye
