#pragma once
#include <atomic>

namespace deadeye {

class Pipeline {
 public:
  Pipeline(int inum) : inum_(inum) {}
  void Quit();
  void Run();

 private:
  int inum_;
  std::atomic<bool> quit_{false};
};

}  // namespace deadeye
