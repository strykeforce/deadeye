#pragma once

namespace deadeye {

struct Command {
  enum class type { start, stop };
  const type type;
  const int param;
};

class Link {
 public:
  virtual void Init(){};
  virtual bool Poll() = 0;
  Command Get() { return command; }
  operator Command() { return command; };

 protected:
  Command command{Command::type::start, 0};
};

}  // namespace deadeye
