#pragma once

#include <atomic>
#include <opencv2/opencv.hpp>
#include <vector>
#include "pipeline.hpp"

namespace deadeye {

class DriverPipeline : public Pipeline {
 public:
  DriverPipeline(int inum);
  virtual ~DriverPipeline();
  void CancelTask() override { cancel_ = true; }
  void UpdateConfig(PipelineConfig *config) override;
  void UpdateStream(StreamConfig *config) override;
  void Run() override;

 protected:
  virtual std::string ToString() const override;

 private:
  std::atomic<bool> cancel_{false};
  PipelineConfig *pipeline_config_{nullptr};
  std::atomic<bool> stream_enabled_;
  cv::VideoCapture GetVideoCapture();
};

}  // namespace deadeye
