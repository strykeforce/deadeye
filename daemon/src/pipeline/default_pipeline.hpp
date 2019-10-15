#pragma once
#include "base_pipeline.hpp"

namespace deadeye {
class DefaultPipeline : public BasePipeline<DefaultPipeline> {
 public:
  DefaultPipeline(int inum) : BasePipeline<DefaultPipeline>{inum} {}

  // required by BasePipeline
  cv::Mat PreProcessFrame(cv::Mat const &frame);
  // required by BasePipeline

  void FilterContours(std::vector<std::vector<cv::Point>> const &src,
                      std::vector<std::vector<cv::Point>> &dest);

 protected:
  cv::VideoCapture GetVideoCapture() override;
  virtual std::string ToString() const override;
};
}  // namespace deadeye
