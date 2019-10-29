#pragma once

#include <atomic>
#include <opencv2/opencv.hpp>
#include <vector>
#include "pipeline.h"

namespace deadeye {

class DriverPipeline : public Pipeline {
 public:
  DriverPipeline(int inum);

  void CancelTask() final;
  void ConfigPipeline(PipelineConfig const &config) final;
  void ConfigStream(StreamConfig const &config) final;
  void Run() final;

 protected:
  std::string ToString() const final;

 private:
  std::string id_;
  std::atomic<bool> cancel_{false};
  PipelineConfig pipeline_config_;
  std::atomic<bool> stream_enabled_;
  cv::VideoCapture GetVideoCapture();
};

}  // namespace deadeye
