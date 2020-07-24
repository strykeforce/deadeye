#pragma once
#include "pipeline/abstract_pipeline.h"

namespace deadeye {
class UprightRectPipeline : public AbstractPipeline {
 public:
  UprightRectPipeline(int inum);

  void Configure(const CaptureConfig& config) override;

  TargetDataPtr ProcessTarget(Contours const& contours) final;

 protected:
  std::string ToString() const final;

 private:
  cv::Point center_;
  std::string capture_type_{"unknown"};
};
}  // namespace deadeye
