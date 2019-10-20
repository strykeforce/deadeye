#pragma once
#include <fmt/core.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <exception>

namespace deadeye {
struct PipelineConfig;
struct StreamConfig;

class PipelineException : public std::exception {
  char const *what_;

 public:
  PipelineException(char const *what) : what_{what} {}
  const char *what() const throw() { return what_; }
};

class Pipeline {
 public:
  Pipeline(int inum) : inum_(inum) {}
  virtual ~Pipeline() {}
  int GetInum() { return inum_; }
  virtual void Run() = 0;
  virtual void CancelTask() = 0;
  virtual void UpdateConfig(PipelineConfig *config) = 0;
  virtual void UpdateStream(StreamConfig *config) = 0;

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
  void UpdateConfig(PipelineConfig *) override {}
  void UpdateStream(StreamConfig *) override {}
  void Run() override { spdlog::warn("{}: Run not implemented", *this); }

 protected:
  virtual std::string ToString() const override {
    return fmt::format("NullPipeline<{}>", inum_);
  }
};
}  // namespace deadeye
