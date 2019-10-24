#pragma once

#include <safe/lockable.h>
#include <atomic>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>

#include "config/pipeline_config.hpp"
#include "config/stream_config.hpp"
#include "link/target_data.hpp"
#include "pipeline/pipeline.hpp"

namespace deadeye {
using LockableStreamConfig = safe::Lockable<StreamConfig>;
using LockablePipelineConfig = safe::Lockable<PipelineConfig>;

class AbstractPipeline : public Pipeline {
 public:
  AbstractPipeline(int inum);
  virtual ~AbstractPipeline();
  void UpdateConfig(PipelineConfig const &config) override;
  void UpdateStream(StreamConfig const &config) override;
  void CancelTask() override;
  virtual void Run() override;

 protected:
  virtual bool StartCapture() = 0;
  virtual void StopCapture() = 0;
  virtual bool GrabFrame(cv::Mat &frame) = 0;
  virtual void ProcessFrame(cv::Mat const &frame);
  virtual void FilterContours(Contours const &src, Contours &dest);
  virtual TargetDataPtr ProcessTarget(Contours const &contours) = 0;

  std::string id_;
  std::atomic<bool> cancel_{false};

  LockableStreamConfig stream_config_;
  std::atomic<bool> stream_config_ready_;
  LockablePipelineConfig pipeline_config_;
  std::atomic<bool> pipeline_config_ready_;

 private:
  void LogTickMeter(cv::TickMeter tm);
};

}  // namespace deadeye
