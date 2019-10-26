#pragma once
#include "abstract_pipeline.h"

namespace deadeye {
class DefaultPipeline : public AbstractPipeline {
 public:
  DefaultPipeline(int inum);
  virtual ~DefaultPipeline() = default;

  virtual void FilterContours(Contours const &src, Contours &dest) override;
  virtual TargetDataPtr ProcessTarget(Contours const &contours) override;

 protected:
  virtual bool StartCapture() override;
  virtual void StopCapture() override;
  virtual bool GrabFrame(cv::Mat &frame) override;
  virtual std::string ToString() const override;

  cv::VideoCapture cap_;
};
}  // namespace deadeye
