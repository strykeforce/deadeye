#pragma once

#include "pipeline/abstract_pipeline.hpp"

namespace deadeye {
class TestPatternPipeline : public AbstractPipeline {
 public:
  TestPatternPipeline(int inum);
  virtual ~TestPatternPipeline() = default;
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
