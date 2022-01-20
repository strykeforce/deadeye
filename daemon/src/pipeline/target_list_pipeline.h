#pragma once
#include "pipeline/abstract_pipeline.h"

namespace deadeye {
class TargetListPipeline : public AbstractPipeline {
 public:
  [[maybe_unused]] TargetListPipeline(int inum, std::string name);

  void Configure(const CaptureConfig& config) override;

  std::unique_ptr<TargetData> ProcessContours(Contours const& contours) final;

 protected:
  [[nodiscard]] std::string ToString() const final;

 private:
  std::string capture_type_{"unknown"};
};
}  // namespace deadeye
