#pragma once
#include "abstract_pipeline.hpp"

namespace deadeye {
class TestPatternPipeline : public AbstractPipeline {
 public:
  TestPatternPipeline(int inum) : AbstractPipeline{inum} {}

  virtual cv::Mat PreProcessFrame(cv::Mat const &frame) override {
    return frame;
  }

  virtual void FilterContours(Contours const &src, Contours &dest) override;

 protected:
  virtual cv::VideoCapture GetVideoCapture() override;
  virtual std::string ToString() const override;
};
}  // namespace deadeye
