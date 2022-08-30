// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#if defined(__aarch64__) && defined(__linux__)
#include <gpiod.hpp>
#endif

namespace deadeye {

class LedDrive {
 public:
  explicit LedDrive(int inum);

  void On();
  void Off();

 private:
#if defined(__aarch64__) && defined(__linux__)
  gpiod::line line_;
#endif
};

}  // namespace deadeye
