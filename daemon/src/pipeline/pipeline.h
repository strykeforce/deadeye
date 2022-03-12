#pragma once
#include <fmt/core.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include <opencv2/core/types.hpp>
#include <utility>

#include "config.h"
#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "link/target_data.h"

namespace deadeye {

using Contours = std::vector<std::vector<cv::Point>>;

class Pipeline {
 public:
  Pipeline(int inum, std::string name) : inum_(inum), name_(std::move(name)) {}
  virtual ~Pipeline() = default;
  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(const Pipeline&) = delete;

  [[nodiscard]] int GetInum() const { return inum_; }
  [[nodiscard]] std::string GetName() const { return name_; }

  [[nodiscard]] virtual cv::Mat GetMask() const = 0;
  virtual void Configure(const CaptureConfig& config) = 0;
  virtual void Configure(const PipelineConfig& config) = 0;

  [[nodiscard]] virtual Contours GetContours() const = 0;
  [[nodiscard]] virtual Contours GetFilteredContours() const = 0;

  virtual std::unique_ptr<TargetData> ProcessFrame(const cv::Mat& frame) = 0;

  template <typename OStream>
  friend OStream& operator<<(OStream& os, const Pipeline& p) {
    os << p.ToString();
    return os;
  }

 protected:
  [[nodiscard]] virtual std::string ToString() const = 0;
  int inum_;
  std::string name_;
};
}  // namespace deadeye
