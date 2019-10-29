#pragma once
#include <ntcore.h>
#include <array>
#include <memory>

namespace deadeye {

class Pipeline;
using PipelinesPtr = std::array<std::unique_ptr<Pipeline>, 5>* const;

class Controller {
 public:
  static void Initialize(std::array<std::unique_ptr<Pipeline>, 5> pipelines) {
    getInstanceImpl(&pipelines);
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

  void StartPoller();

  NT_Inst inst_;
  NT_EntryListenerPoller poller_;
  NT_EntryListener entry_listener_;
  std::array<bool, 5> has_active_pipeline_;
};
}  // namespace deadeye
