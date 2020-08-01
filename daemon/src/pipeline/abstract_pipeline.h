#pragma once

#include <opencv2/core/mat.hpp>

#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "link/target_data.h"
#include "pipeline/pipeline.h"

namespace deadeye {

class AbstractPipeline : public Pipeline {
 public:
  AbstractPipeline(int inum, std::string name);

  void Configure(const CaptureConfig& config) override;
  void Configure(const PipelineConfig& config) override;

  virtual cv::Mat GetMask() const override { return hsv_threshold_output_; }

  virtual Contours GetContours() const override {
    return find_contours_output_;
  }

  virtual Contours GetFilteredContours() const override {
    return filter_contours_output_;
  }

  virtual TargetDataPtr ProcessFrame(const cv::Mat& frame) override;

 protected:
  virtual void FilterContours(const FilterConfig& filter, const Contours& src,
                              Contours& dest);
  virtual TargetDataPtr ProcessTarget(const Contours& contours);

  virtual std::string ToString() const override;

  std::string id_;

  PipelineConfig pipeline_config_;

 private:
  cv::Mat frame_;
  cv::Mat hsv_threshold_output_;
  Contours find_contours_output_;
  Contours filter_contours_output_;
  std::string capture_type_{"unknown"};
};

}  // namespace deadeye
