#pragma once

#include "pipeline/abstract_pipeline.hpp"

namespace deadeye {
class TestPatternPipeline : public AbstractPipeline {
 public:
  TestPatternPipeline(int inum) : AbstractPipeline{inum} {}

  virtual cv::Mat PreProcessFrame(cv::Mat const &frame) override {
    return frame;
  }

  virtual void FilterContours(Contours const &src, Contours &dest) override;
  virtual std::unique_ptr<TargetData> ProcessTarget(
      Contours const &contours) override;

 protected:
  virtual cv::VideoCapture GetVideoCapture() override;
  virtual std::string ToString() const override;
};
}  // namespace deadeye
