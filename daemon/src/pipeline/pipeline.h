#pragma once
#include <fmt/core.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include <opencv2/core/types.hpp>

#include "config/capture_config.h"
#include "config/deadeye_config.h"
#include "config/pipeline_config.h"
#include "link/target_data.h"

namespace deadeye {

using Contours = std::vector<std::vector<cv::Point>>;
using TargetDataPtr = std::unique_ptr<TargetData>;

class Pipeline {
 public:
  Pipeline(int inum) : inum_(inum) {}
  virtual ~Pipeline() = default;
  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(const Pipeline&) = delete;

  int GetInum() const { return inum_; }

  virtual cv::Mat GetMask() const = 0;
  virtual void Configure(const CaptureConfig& config) = 0;
  virtual void Configure(const PipelineConfig& config) = 0;

  virtual Contours GetContours() const = 0;
  virtual Contours GetFilteredContours() const = 0;

  virtual TargetDataPtr ProcessFrame(const cv::Mat& frame) = 0;

  template <typename OStream>
  friend OStream& operator<<(OStream& os, const Pipeline& p) {
    os << p.ToString();
    return os;
  }

 protected:
  virtual std::string ToString() const = 0;
  int inum_;
};
}  // namespace deadeye
