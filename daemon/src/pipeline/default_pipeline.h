#pragma once
#include "abstract_pipeline.h"

namespace deadeye {
class DefaultPipeline : public AbstractPipeline {
 public:
  DefaultPipeline(int inum);

  void FilterContours(FilterConfig const &filter, Contours const &src,
                      Contours &dest) final;
  virtual TargetDataPtr ProcessTarget(Contours const &contours) = 0;

 protected:
  bool StartCapture() final;
  void StopCapture() final;
  bool GrabFrame(cv::Mat &frame) final;
  virtual std::string ToString() const;

  cv::VideoCapture cap_;
  cv::Point center_;
  cv::Point2f center2f_;
  double frame_area_;
};
}  // namespace deadeye
