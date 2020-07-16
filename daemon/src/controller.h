#pragma once
#include <ntcore.h>

#include <array>
#include <memory>

namespace deadeye {

static constexpr int kNumCameras = 5;

class Pipeline;
using Pipelines = std::array<std::unique_ptr<Pipeline>, kNumCameras>;
using PipelinesPtr = Pipelines* const;

class Controller {
 public:
  static Controller& Initialize(Pipelines pipelines) {
    return getInstanceImpl(&pipelines);
  }
  static Controller& GetInstance() { return getInstanceImpl(); }

  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;
  ~Controller();
  void Run();
  void ShutDown();

 private:
  static Controller& getInstanceImpl(PipelinesPtr pipelines = nullptr) {
    static Controller instance{pipelines};
    return instance;
  }
  Controller(PipelinesPtr pipelines);

  void StartNetworkTables();

  void InitializeNetworkTables();

  template <int inum>
  void InitializeCamera();

  template <int inum>
  void LogCamera();

  void StartPoller();

  NT_Inst inst_;
  NT_EntryListenerPoller poller_;
  NT_EntryListener entry_listener_;
  std::array<bool, kNumCameras> has_active_pipeline_;
};
}  // namespace deadeye
