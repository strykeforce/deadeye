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

  void Configure(const CaptureConfig& config) override;
  void Configure(const PipelineConfig& config) override;
  void Configure(const StreamConfig& config) override;

  Contours GetContours() override;
  Contours GetFilteredContours() override;

  virtual TargetDataPtr ProcessFrame(const cv::Mat& frame) override;

  virtual void ProcessStreamFrame(cv::Mat& preview,
                                  const TargetData* target_data) override;

 protected:
  virtual void FilterContours(const FilterConfig& filter, const Contours& src,
                              Contours& dest);
  virtual TargetDataPtr ProcessTarget(const Contours& contours);

  std::string id_;
  std::string pipeline_type_{"unknown"};

 protected:
  virtual std::string ToString() const override;

  CaptureConfig capture_config_;
  PipelineConfig pipeline_config_;
  StreamConfig stream_config_;

  // remove?
  cv::Point center_;
  cv::Point2f center2f_;
  double frame_area_;

 private:
  cv::Mat frame_;
  cv::Mat hsv_threshold_output_;
  Contours find_contours_output_;
  Contours filter_contours_output_;
  int preview_border_;
  bool preview_resize_;
};

}  // namespace deadeye
