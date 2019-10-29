#pragma once

#include <cscore.h>
#include <safe/lockable.h>
#include <atomic>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>

#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "link/target_data.h"
#include "pipeline/pipeline.h"

namespace deadeye {
using LockableStreamConfig = safe::Lockable<StreamConfig>;
using LockablePipelineConfig = safe::Lockable<PipelineConfig>;

class AbstractPipeline : public Pipeline {
 public:
  AbstractPipeline(int inum);
  virtual ~AbstractPipeline() = default;
  void ConfigPipeline(PipelineConfig const &config) override;
  void ConfigStream(StreamConfig const &config) override;
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
  std::string pipeline_type_{"unknown"};

 private:
  void StreamFrame();
  void LogTickMeter(cv::TickMeter tm);

  inline bool StreamEnabled() {
    return !(view_ == StreamConfig::View::none &&
             contour_ == StreamConfig::Contour::none);
  }

  cv::Mat frame_;
  cv::Mat hsv_threshold_output_;
  Contours filter_contours_output_;
  Contours find_contours_output_;
  StreamConfig::View view_;
  StreamConfig::Contour contour_;
  cs::CvSource cvsource_;
  TargetDataPtr target_data_;
};

}  // namespace deadeye
