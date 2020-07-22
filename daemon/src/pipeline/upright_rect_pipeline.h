#pragma once
#include "pipeline/abstract_pipeline.h"

namespace deadeye {
class UprightRectPipeline : public AbstractPipeline {
 public:
  UprightRectPipeline(int inum);

  TargetDataPtr ProcessTarget(Contours const& contours) final;

 protected:
  std::string ToString() const final;
};
}  // namespace deadeye
