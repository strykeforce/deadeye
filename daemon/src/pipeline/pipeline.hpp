#pragma once
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
  int inum_;

  virtual std::string ToString() const {
    return "Pipeline<" + std::to_string(inum_) + ">";
  }
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
    return "NullPipeline<" + std::to_string(inum_) + ">";
  }
};
}  // namespace deadeye
