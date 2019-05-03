#pragma once

class Link {
  enum class command { start, stop };

 public:
  virtual bool poll() = 0;
  virtual command get() = 0;
  virtual command(){return get()};
};
