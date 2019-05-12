#pragma once
#include "pipeline.hpp"

namespace deadeye {
class DefaultPipeline : public Pipeline<DefaultPipeline> {
 public:
  DefaultPipeline(int inum) : Pipeline<DefaultPipeline>{inum} {}
  void FindContours(std::vector<std::vector<cv::Point>> const &src,
                    std::vector<std::vector<cv::Point>> &dest);
};
}  // namespace deadeye
