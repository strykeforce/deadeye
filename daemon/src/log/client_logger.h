#pragma once
#include <networktables/NetworkTableEntry.h>

#include <string_view>

namespace deadeye {
class ClientLogger {
 public:
  enum class Level { debug, info, warn, error };

  explicit ClientLogger(int inum);
  ClientLogger(int inum, nt::NetworkTableInstance nti);

  void Log(Level level, std::string_view msg);

  inline void Debug(std::string_view msg) { Log(Level::debug, msg); }
  inline void Info(std::string_view msg) { Log(Level::info, msg); }
  inline void Warn(std::string_view msg) { Log(Level::warn, msg); }
  inline void Error(std::string_view msg) { Log(Level::error, msg); }

 private:
  nt::NetworkTableEntry entry_;
};
}  // namespace deadeye
