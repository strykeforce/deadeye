#pragma once

#include <ntcore.h>
#include <spdlog/spdlog.h>

#define DE_UNIT "A"

#define DE_DEADEYE_TABLE "/Deadeye"
#define DE_CAMERA "/Camera"
#define DE_LIGHTS "/Lights"
#define DE_CONTROL_TABLE DE_DEADEYE_TABLE "/Control" DE_UNIT
#define DE_CONFIG_TABLE DE_DEADEYE_TABLE "/Config" DE_UNIT

#define DE_CAMERA_CONTROL_TABLE(inum) DE_CONTROL_TABLE DE_CAMERA inum
#define DE_LIGHTS_CONTROL_TABLE(inum) DE_CAMERA_CONTROL_TABLE(inum) DE_LIGHTS
#define DE_CAMERA_CONFIG_ENTRY(inum) DE_CONFIG_TABLE DE_CAMERA inum

#define DE_ERROR "Error"
#define DE_ON "On"
#define DE_OFF "Off"
#define DE_BLINK "Blink"

#define DE_CAMERA_CONTROL(inum, param) DE_CAMERA_CONTROL_TABLE(inum) "/" param
#define DE_LIGHTS_CONTROL(inum, param) DE_LIGHTS_CONTROL_TABLE(inum) "/" param

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
