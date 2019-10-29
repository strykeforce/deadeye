#pragma once

#include <atomic>
#include <opencv2/opencv.hpp>
#include <vector>
#include "pipeline.h"

namespace deadeye {

class DriverPipeline : public Pipeline {
 public:
  DriverPipeline(int inum);
  virtual ~DriverPipeline() = default;
  void CancelTask() override { cancel_ = true; }
  void ConfigPipeline(PipelineConfig const &config) override;
  void ConfigStream(StreamConfig const &config) override;
  void Run() override;

 protected:
  virtual std::string ToString() const override;

 private:
  std::string id_;
  std::atomic<bool> cancel_{false};
  PipelineConfig pipeline_config_;
  std::atomic<bool> stream_enabled_;
  cv::VideoCapture GetVideoCapture();
};

}  // namespace deadeye
