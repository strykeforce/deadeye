#pragma once

#include "config/deadeye_config.h"
#include "link/target_data.h"
#include "pipeline/pipeline.h"

namespace deadeye {
class NullPipeline : public Pipeline {
 public:
  NullPipeline(int inum)
      : Pipeline{inum}, id_{DEADEYE_UNIT + std::to_string(inum_)} {}

  virtual void ConfigCapture(CaptureConfig const &config) final {}
  virtual void ConfigPipeline(PipelineConfig const &config) final {}
  virtual void ConfigStream(StreamConfig const &config) final {}

  virtual bool StartCapture() final { return false; }
  virtual void StopCapture() final {}

  virtual bool GrabFrame(cv::Mat &frame) final { return false; }

  virtual TargetDataPtr ProcessFrame(cv::Mat const &frame) final {
    return std::make_unique<TargetData>(id_, 0, false);
  };

  virtual Contours GetContours() final { return Contours{}; }
  virtual Contours GetFilteredContours() final { return Contours{}; }

  virtual void ProcessStreamFrame(cv::Mat &preview,
                                  TargetData const *target_data) final {}

 protected:
  virtual std::string ToString() const override {
    return fmt::format("NullPipeline<{}>", id_);
  }
  std::string id_;
};
}  // namespace deadeye