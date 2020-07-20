#pragma once
#include "pipeline/abstract_pipeline.h"

namespace deadeye {
class MinAreaRectPipeline : public AbstractPipeline {
 public:
  MinAreaRectPipeline(int inum);

  TargetDataPtr ProcessTarget(Contours const& contours) final;

 protected:
  std::string ToString() const final;
};
}  // namespace deadeye
