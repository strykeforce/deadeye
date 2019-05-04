#pragma once

namespace deadeye {

struct Command {
  enum class type { start, stop };
  const type type;
  const int param;
};

class Link {
 public:
  virtual void init(){};
  virtual bool poll() = 0;
  Command get() { return command; }
  operator Command() { return command; };

 protected:
  Command command{Command::type::start, 0};
};

}  // namespace deadeye
