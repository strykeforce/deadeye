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

  virtual void ConfigCapture(CaptureConfig const &config) = 0;
  virtual void ConfigPipeline(PipelineConfig const &config) = 0;
  virtual void ConfigStream(StreamConfig const &config) = 0;

  virtual bool StartCapture() = 0;
  virtual void StopCapture() = 0;

  virtual bool GrabFrame(cv::Mat &frame) = 0;

  virtual TargetDataPtr ProcessFrame(cv::Mat const &frame) = 0;

  virtual Contours GetFilteredContours() = 0;

  virtual void ProcessStreamFrame(cv::Mat &preview,
                                  TargetData const *target_data) = 0;

  template <typename OStream>
  friend OStream &operator<<(OStream &os, Pipeline const &p) {
    os << p.ToString();
    return os;
  }

 protected:
  virtual std::string ToString() const = 0;
  int inum_;
};
}  // namespace deadeye
