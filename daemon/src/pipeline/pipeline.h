#pragma once
#include <fmt/core.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include <opencv2/core/types.hpp>

#include "config/capture_config.h"
#include "config/deadeye_config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "link/target_data.h"

namespace deadeye {

using Contours = std::vector<std::vector<cv::Point>>;
using TargetDataPtr = std::unique_ptr<TargetData>;

class Pipeline {
 public:
  Pipeline(int inum) : inum_(inum) {}
  virtual ~Pipeline() = default;
  Pipeline(Pipeline const &) = delete;
  Pipeline &operator=(Pipeline const &) = delete;

  int GetInum() { return inum_; }
  virtual void ConfigCapture(CaptureConfig const &config) {}
  virtual void ConfigPipeline(PipelineConfig const &config) {}
  virtual void ConfigStream(StreamConfig const &config) {}

  virtual bool StartCapture() { return false; };
  virtual void StopCapture(){};
  virtual bool GrabFrame(cv::Mat &frame) { return false; };
  virtual TargetDataPtr ProcessFrame(cv::Mat const &frame) = 0;
  virtual Contours GetFilteredContours() { return Contours{}; }
  virtual void ProcessStreamFrame(cv::Mat &preview,
                                  TargetData const *target_data) {}

  template <typename OStream>
  friend OStream &operator<<(OStream &os, Pipeline const &p) {
    os << p.ToString();
    return os;
  }

 protected:
  virtual std::string ToString() const = 0;
  int inum_;
};

class NullPipeline : public Pipeline {
 public:
  NullPipeline(int inum)
      : Pipeline{inum}, id_{DEADEYE_UNIT + std::to_string(inum_)} {}
  virtual TargetDataPtr ProcessFrame(cv::Mat const &frame) override {
    return std::make_unique<TargetData>(id_, 0, false);
  };

 protected:
  virtual std::string ToString() const override {
    return fmt::format("NullPipeline<{}>", id_);
  }
  std::string id_;
};
}  // namespace deadeye
