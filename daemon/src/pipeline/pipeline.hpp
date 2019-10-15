#pragma once
#include <spdlog/fmt/ostr.h>
#include <exception>
#include <iostream>

#include "config/pipeline_config.hpp"
#include "config/stream_config.hpp"

namespace deadeye {
class PipelineException : public std::exception {
  char const *what_;

 public:
  PipelineException(char const *what) : what_{what} {}
  const char *what() const throw() { return what_; }
};

class Pipeline {
 public:
  Pipeline(int inum) : inum_{inum} {}
  virtual ~Pipeline() {}
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
}  // namespace deadeye
