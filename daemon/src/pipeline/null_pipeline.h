// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <memory>
#include <string>

#include "config.h"
#include "link/target_data.h"
#include "pipeline/pipeline.h"

namespace deadeye {
class NullPipeline : public Pipeline {
 public:
  NullPipeline(int inum, std::string name)
      : Pipeline{inum, name}, id_{DEADEYE_UNIT + std::to_string(inum_)} {}

  explicit NullPipeline(int inum) : NullPipeline(inum, "NullPipeline") {}

  void Configure(CaptureConfig const& config) override {}
  void Configure(PipelineConfig const& config) override {}

  [[nodiscard]] cv::Mat GetMask() const override { return {}; }
  [[nodiscard]] Contours GetContours() const override { return Contours{}; }
  [[nodiscard]] Contours GetFilteredContours() const override {
    return Contours{};
  }

  std::unique_ptr<TargetData> ProcessFrame(cv::Mat const& frame) override {
    return std::make_unique<TargetData>(id_, 0, false);
  };

 protected:
  [[nodiscard]] std::string ToString() const override {
    return fmt::format("NullPipeline<{}>", id_);
  }
  std::string id_;
};
}  // namespace deadeye
