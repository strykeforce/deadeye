#pragma once
#include "abstract_pipeline.h"

namespace deadeye {
class DefaultPipeline : public AbstractPipeline {
 public:
  DefaultPipeline(int inum);

  bool StartCapture() override;
  void StopCapture() override;

  bool GrabFrame(cv::Mat& frame) override;

  void FilterContours(const FilterConfig& filter, const Contours& src,
                      Contours& dest) override;

 protected:
  TargetDataPtr ProcessTarget(const Contours& contours) override;
  virtual std::string ToString() const override;

  cv::VideoCapture cap_;
  cv::Point center_;
  cv::Point2f center2f_;
  double frame_area_;
};
}  // namespace deadeye
