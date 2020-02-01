#pragma once
#include "default_pipeline.h"

namespace deadeye {
class MinAreaRectPipeline : public DefaultPipeline {
 public:
  MinAreaRectPipeline(int inum);

  TargetDataPtr ProcessTarget(Contours const &contours) final;

 protected:
  std::string ToString() const final;
};
}  // namespace deadeye
