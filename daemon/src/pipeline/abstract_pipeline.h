#pragma once

#include <cscore.h>
#include <cscore_cv.h>
#include <safe/lockable.h>
#include <atomic>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>

#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "link/target_data.h"
#include "pipeline/pipeline.h"

namespace deadeye {
using LockableCaptureConfig = safe::Lockable<CaptureConfig>;
using LockablePipelineConfig = safe::Lockable<PipelineConfig>;
using LockableStreamConfig = safe::Lockable<StreamConfig>;

class AbstractPipeline : public Pipeline {
 public:
  AbstractPipeline(int inum);

  void ConfigCapture(CaptureConfig const &config) final;
  void ConfigPipeline(PipelineConfig const &config) final;
  void ConfigStream(StreamConfig const &config) final;
  void CancelTask() final;
  void Run() final;

 protected:
  virtual bool StartCapture() = 0;
  virtual void StopCapture() = 0;
  virtual bool GrabFrame(cv::Mat &frame) = 0;
  virtual void ProcessFrame(cv::Mat const &frame) {}
  virtual void FilterContours(Contours const &src, Contours &dest) {}
  virtual TargetDataPtr ProcessTarget(Contours const &contours) = 0;

  std::string id_;
  std::atomic<bool> cancel_{false};

  LockableCaptureConfig capture_config_;
  std::atomic<bool> capture_config_ready_{false};
  LockableStreamConfig stream_config_;
  std::atomic<bool> stream_config_ready_{false};
  LockablePipelineConfig pipeline_config_;
  std::atomic<bool> pipeline_config_ready_{false};
  std::string pipeline_type_{"unknown"};
  int fps_;

 private:
  void StreamFrame();
  void LogFrame();
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
  bool log_enabled_;
};

}  // namespace deadeye
