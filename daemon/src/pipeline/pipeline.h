#pragma once
#include <fmt/core.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <exception>
#include <opencv2/core/types.hpp>

#include "config/capture_config.h"
#include "config/pipeline_config.h"
#include "config/stream_config.h"

namespace deadeye {
struct TargetData;

using Contours = std::vector<std::vector<cv::Point>>;
using TargetDataPtr = std::unique_ptr<TargetData>;

class PipelineException : public std::exception {
  char const *what_;

 public:
  PipelineException(char const *what) : what_{what} {}
  const char *what() const throw() { return what_; }
};

class Pipeline {
 public:
  Pipeline(int inum) : inum_(inum) {}
  virtual ~Pipeline() = default;
  Pipeline(Pipeline const &) = delete;
  Pipeline &operator=(Pipeline const &) = delete;

  int GetInum() { return inum_; }
  virtual void Run() = 0;
  virtual void CancelTask() = 0;
  virtual void ConfigCapture(CaptureConfig const &config) {}
  virtual void ConfigPipeline(PipelineConfig const &config) {}
  virtual void ConfigStream(StreamConfig const &config) {}

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
  NullPipeline(int inum) : Pipeline{inum} {}
  void CancelTask() override {}
  void Run() override { spdlog::warn("{}: Run not implemented", *this); }

 protected:
  virtual std::string ToString() const override {
    return fmt::format("NullPipeline<{}>", inum_);
  }
};
}  // namespace deadeye
