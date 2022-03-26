// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <memory>
#include <string>

#include "pipeline/abstract_pipeline.h"

namespace deadeye {
class TargetListPipeline : public AbstractPipeline {
 public:
  [[maybe_unused]] TargetListPipeline(int inum, std::string name);

  std::unique_ptr<TargetData> ProcessContours(Contours const& contours) final;

 protected:
  [[nodiscard]] std::string ToString() const final;
};
}  // namespace deadeye
