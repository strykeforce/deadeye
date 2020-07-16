#pragma once
#include <exception>

namespace deadeye {
class PipelineException : public std::exception {
  char const *what_;

 public:
  PipelineException(char const *what) : what_{what} {}
  const char *what() const throw() { return what_; }
};
}  // namespace deadeye