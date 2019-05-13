#include "default_pipeline.hpp"

using namespace deadeye;

struct MyException : public std::exception {
  const char *what() const throw() { return "C++ Exception"; }
};

void DefaultPipeline::FindContours(
    std::vector<std::vector<cv::Point>> const &src,
    std::vector<std::vector<cv::Point>> &dest) {
  //  throw MyException();
}
