#pragma once
#include <array>

#include <ntcore.h>

namespace deadeye {
struct LinkConfig;

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

  LinkConfig GetLinkConfig();

 private:
  Controller();
  void StartNetworkTables();
  void InitializeNetworkTableEntries();
  void InitializeCameraConfig();
  void StartPoller();

  NT_Inst inst_;
  NT_EntryListenerPoller poller_;
  NT_EntryListener entry_listener_;
};
}  // namespace deadeye
