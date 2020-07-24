#pragma once
#include "pipeline/abstract_pipeline.h"

namespace deadeye {
class MinAreaRectPipeline : public AbstractPipeline {
 public:
  MinAreaRectPipeline(int inum);

  void Configure(const CaptureConfig& config) override;

  TargetDataPtr ProcessTarget(Contours const& contours) final;

 protected:
  std::string ToString() const final;

 private:
  cv::Point2f center2f_;
  std::string capture_type_{"unknown"};
};
}  // namespace deadeye
