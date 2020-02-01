#pragma once
#include "default_pipeline.h"

namespace deadeye {
class UprightRectPipeline : public DefaultPipeline {
 public:
  UprightRectPipeline(int inum);

  TargetDataPtr ProcessTarget(Contours const &contours) final;

 protected:
  std::string ToString() const final;
};
}  // namespace deadeye
