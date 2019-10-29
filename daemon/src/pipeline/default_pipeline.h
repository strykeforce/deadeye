#pragma once
#include "abstract_pipeline.h"

namespace deadeye {
class DefaultPipeline : public AbstractPipeline {
 public:
  DefaultPipeline(int inum);

  void FilterContours(Contours const &src, Contours &dest) final;
  TargetDataPtr ProcessTarget(Contours const &contours) final;

 protected:
  bool StartCapture() final;
  void StopCapture() final;
  bool GrabFrame(cv::Mat &frame) final;
  std::string ToString() const final;

  cv::VideoCapture cap_;
};
}  // namespace deadeye
