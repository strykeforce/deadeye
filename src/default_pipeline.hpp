#pragma once
#include "base_pipeline.hpp"

namespace deadeye {
class DefaultPipeline : public BasePipeline<DefaultPipeline> {
 public:
  DefaultPipeline(int inum) : BasePipeline<DefaultPipeline>{inum} {}
  void FindContours(std::vector<std::vector<cv::Point>> const &src,
                    std::vector<std::vector<cv::Point>> &dest);
};
}  // namespace deadeye
