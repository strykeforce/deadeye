#pragma once

#include <ntcore.h>
#include <spdlog/spdlog.h>

#define DE_STRINGIFY(s) DE_STR(s)
#define DE_STR(s) #s

#define DEADEYE_UNIT DE_STRINGIFY(DEADEYE_UNIT_ID)

#define DE_DEADEYE_TABLE "/Deadeye"
#define DE_CONFIG "/Config"
#define DE_STREAM "/Stream"
#define DE_LIGHTS "/Light"
#define DE_CONTROL_TABLE DE_DEADEYE_TABLE "/" DEADEYE_UNIT
#define DE_CONFIG_TABLE DE_DEADEYE_TABLE "/" DEADEYE_UNIT

#define DE_CAMERA_CONTROL_TABLE(inum) DE_CONTROL_TABLE "/" inum
#define DE_LIGHTS_CONTROL_TABLE(inum) DE_CAMERA_CONTROL_TABLE(inum) DE_LIGHTS
#define DE_CLIENT_TABLE DE_DEADEYE_TABLE "/Client"

#define DE_ERROR "Error"
#define DE_ON "On"
#define DE_OFF "Off"
#define DE_BLINK "Blink"
#define DE_ADDRESS "Address"
#define DE_PORT "Port"

#define DE_CAMERA_CONTROL(inum, param) DE_CAMERA_CONTROL_TABLE(inum) "/" param
#define DE_LIGHTS_CONTROL(inum, param) DE_LIGHTS_CONTROL_TABLE(inum) "/" param
#define DE_CAMERA_CONFIG_ENTRY(inum) DE_CONFIG_TABLE "/" inum DE_CONFIG
#define DE_STREAM_CONFIG_ENTRY(inum) DE_CONFIG_TABLE "/" inum DE_STREAM
#define DE_CLIENT_ADDRESS_ENTRY DE_CLIENT_TABLE "/" DE_ADDRESS
#define DE_CLIENT_PORT_ENTRY DE_CLIENT_TABLE "/" DE_PORT

#define CLIENT_PORT DEADEYE_CLIENT_PORT
#define CLIENT_ADDRESS DE_STRINGIFY(DEADEYE_CLIENT_ADDRESS)

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
  std::string GetClientAddress();
  int GetClientPort();

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
