#pragma once
#include <atomic>
#include <sstream>
#include <thread>
#include "fmt/format.h"

namespace deadeye {

class Pipeline {
 public:
  Pipeline(int inum) : inum_(inum) {}
  void Quit();
  void Run();

 private:
  friend fmt::formatter<Pipeline>;
  int inum_;
  std::atomic<bool> quit_{false};
};

}  // namespace deadeye

// custom formatting for fmt library
template <>
struct fmt::formatter<deadeye::Pipeline> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const deadeye::Pipeline &p, FormatContext &ctx) {
    return format_to(ctx.out(), "Pipeline<{}> {{ quit_: {} }}", p.inum_,
                     p.quit_.load());
  }
};
