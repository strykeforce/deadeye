#pragma once
#include "base_pipeline.hpp"

namespace deadeye {
class TestPatternPipeline : public BasePipeline<TestPatternPipeline> {
 public:
  TestPatternPipeline(int inum) : BasePipeline<TestPatternPipeline>{inum} {}
  void FindContours(std::vector<std::vector<cv::Point>> const &src,
                    std::vector<std::vector<cv::Point>> &dest);
};
}  // namespace deadeye
