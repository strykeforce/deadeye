// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <memory>
#include <string>

#include "hardware/lights.h"
#include "pipeline/abstract_pipeline.h"

namespace deadeye {
class LatencyPipeline : public AbstractPipeline {
 public:
  [[maybe_unused]] LatencyPipeline(int inum, std::string name);

  std::unique_ptr<TargetData> ProcessContours(Contours const& contours) final;

 protected:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
  [[nodiscard]] std::string ToString() const final;

  inline bool IsLedOn() {
    switch (inum_) {
      case 0:
        return Lights<0>::IsOn();
      case 1:
        return Lights<1>::IsOn();
      case 2:
        return Lights<2>::IsOn();
      case 3:
        return Lights<3>::IsOn();
      case 4:
        return Lights<4>::IsOn();
      default:
        return false;
    }
  }
};
}  // namespace deadeye
