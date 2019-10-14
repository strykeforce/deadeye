#pragma once

#include <atomic>
// #include <mutex>
#include <vector>
#include "pipeline.hpp"

namespace deadeye {

class DriverPipeline : public Pipeline {
 public:
  DriverPipeline(int inum);
  virtual ~DriverPipeline() {}
  void CancelTask() override { cancel_ = true; }
  void UpdateConfig(PipelineConfig *config) override;
  void UpdateStream(StreamConfig *config) override;
  void Run() override;

 private:
  std::atomic<bool> cancel_{false};
};

}  // namespace deadeye
