#pragma once

#include "config.h"
#include "link/target_data.h"
#include "pipeline/pipeline.h"

namespace deadeye {
class NullPipeline : public Pipeline {
 public:
  explicit NullPipeline(int inum)
      : Pipeline{inum, "NullPipeline"},
        id_{DEADEYE_UNIT + std::to_string(inum_)} {}

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