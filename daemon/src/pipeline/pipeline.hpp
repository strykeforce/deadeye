#pragma once
#include <exception>
#include "pipeline_config.hpp"
#include "stream_config.hpp"

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

 protected:
  int inum_;
};
}  // namespace deadeye
