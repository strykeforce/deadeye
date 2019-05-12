#pragma once

namespace deadeye {
class TargetStrategy {
 public:
  virtual ~TargetStrategy() {}
  virtual void Process() = 0;
};
}  // namespace deadeye
