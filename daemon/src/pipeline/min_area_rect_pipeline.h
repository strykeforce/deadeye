#pragma once
#include "pipeline/abstract_pipeline.h"

namespace deadeye {
class MinAreaRectPipeline : public AbstractPipeline {
 public:
  [[maybe_unused]] MinAreaRectPipeline(int inum, std::string name);

  void Configure(const CaptureConfig& config) override;

  std::unique_ptr<TargetData> ProcessContours(Contours const& contours) final;

 protected:
  [[nodiscard]] std::string ToString() const final;

 private:
  cv::Point2f center2f_;
};
}  // namespace deadeye
