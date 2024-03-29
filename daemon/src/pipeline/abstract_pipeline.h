// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <memory>
#include <opencv2/core/mat.hpp>
#include <string>

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

  [[nodiscard]] cv::Mat GetMask() const override {
    return hsv_threshold_output_;
  }

  [[nodiscard]] Contours GetContours() const override {
    return find_contours_output_;
  }

  [[nodiscard]] Contours GetFilteredContours() const override {
    return filter_contours_output_;
  }

  std::unique_ptr<TargetData> ProcessFrame(const cv::Mat& frame) override;

 protected:
  virtual void FilterContours(const FilterConfig& filter, const Contours& src,
                              Contours& dest);
  virtual std::unique_ptr<TargetData> ProcessContours(const Contours& contours);

  [[nodiscard]] std::string ToString() const override;

  std::string_view id_;
  PipelineConfig pipeline_config_;
  std::string capture_type_{"unknown"};

 private:
  std::string id_string_;
  cv::Mat frame_;
  cv::Mat hsv_threshold_output_;
  Contours find_contours_output_;
  Contours filter_contours_output_;
};

}  // namespace deadeye
