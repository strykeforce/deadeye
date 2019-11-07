#pragma once

#ifdef __aarch64__
#include <gpiod.hpp>
#endif

namespace deadeye {

class LedDrive {
 public:
  LedDrive(int inum);

  void On();
  void Off();

 private:
#ifdef __aarch64__
  gpiod::line line_;
#endif
};

}  // namespace deadeye