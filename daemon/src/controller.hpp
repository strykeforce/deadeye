#pragma once
#include <ntcore.h>
#include <array>
#include <memory>

namespace deadeye {

class Pipeline;

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
  static Controller& getInstanceImpl(
      std::array<std::unique_ptr<Pipeline>, 5>* const pipelines = nullptr) {
    static Controller instance{pipelines};
    return instance;
  }
  Controller(std::array<std::unique_ptr<Pipeline>, 5>* const pipelines);

  void StartNetworkTables();

  void InitializeNetworkTableEntries();

  template <int N>
  void InitializeCamera();

  void StartPoller();

  NT_Inst inst_;
  NT_EntryListenerPoller poller_;
  NT_EntryListener entry_listener_;
  std::array<bool, 5> has_active_pipeline_;
};
}  // namespace deadeye
