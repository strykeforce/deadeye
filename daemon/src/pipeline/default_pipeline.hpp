#pragma once
#include "abstract_pipeline.hpp"

namespace deadeye {
class DefaultPipeline : public AbstractPipeline {
 public:
  DefaultPipeline(int inum) : AbstractPipeline{inum} {}

  virtual cv::Mat PreProcessFrame(cv::Mat const &frame) override;

  virtual void FilterContours(Contours const &src, Contours &dest) override;

 protected:
  virtual cv::VideoCapture GetVideoCapture() override;
  virtual std::string ToString() const override;
};
}  // namespace deadeye
