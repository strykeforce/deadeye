#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>

#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "link/target_data.h"
#include "pipeline/pipeline.h"

namespace deadeye {

class AbstractPipeline : public Pipeline {
 public:
  AbstractPipeline(int inum);

  void ConfigCapture(CaptureConfig const &config) final;
  void ConfigPipeline(PipelineConfig const &config) final;
  void ConfigStream(StreamConfig const &config) final;
  Contours GetFilteredContours() final;

 protected:
  virtual TargetDataPtr ProcessFrame(cv::Mat const &frame) override;
  // virtual void ProcessFrame(cv::Mat const &frame) {}
  virtual void FilterContours(FilterConfig const &filter, Contours const &src,
                              Contours &dest) {}
  virtual TargetDataPtr ProcessTarget(Contours const &contours) = 0;
  virtual void ProcessStreamFrame(cv::Mat &preview,
                                  TargetData const *target_data) override;

  std::string id_;
  std::string pipeline_type_{"unknown"};

 protected:
  CaptureConfig capture_config_;
  PipelineConfig pipeline_config_;
  StreamConfig stream_config_;

 private:
  cv::Mat frame_;
  cv::Mat hsv_threshold_output_;
  Contours filter_contours_output_;
  Contours find_contours_output_;
  int preview_border_;
  bool preview_resize_;
};

}  // namespace deadeye
