#pragma once

#include "config/deadeye_config.h"
#include "link/target_data.h"
#include "pipeline/pipeline.h"

namespace deadeye {
class NullPipeline : public Pipeline {
 public:
  NullPipeline(int inum)
      : Pipeline{inum, "NullPipeline"},
        id_{DEADEYE_UNIT + std::to_string(inum_)} {}

  virtual void Configure(CaptureConfig const& config) override {}
  virtual void Configure(PipelineConfig const& config) override {}

  virtual cv::Mat GetMask() const override { return cv::Mat(); }
  virtual Contours GetContours() const override { return Contours{}; }
  virtual Contours GetFilteredContours() const override { return Contours{}; }

  virtual TargetDataPtr ProcessFrame(cv::Mat const& frame) override {
    return std::make_unique<TargetData>(id_, 0, false);
  };

 protected:
  virtual std::string ToString() const override {
    return fmt::format("NullPipeline<{}>", id_);
  }
  std::string id_;
};
}  // namespace deadeye