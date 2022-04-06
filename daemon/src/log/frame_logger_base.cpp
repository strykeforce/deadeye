// Copyright (c) 2022 Stryke Force FRC 2767

#include "log/frame_logger_base.h"

#include <dirent.h>
#include <sys/stat.h>

#include <string>

using ::deadeye::logger::FrameLoggerBase;

FrameLoggerBase::FrameLoggerBase(const int inum, const FrameLogConfig& config,
                                 FrameLoggerState& state,
                                 FrameLoggerQueue& queue,
                                 std::atomic<bool>& cancel)
    : id_{CameraId(inum)},
      enabled_{config.fps > 0 && CheckMount(config) && CheckDir(config)},
      state_{state},
      queue_(queue),
      cancel_{cancel},
      client_logger_{inum} {
  base_template_ =
      fmt::format("{}/{}/{{:04d}}-{{{{:04d}}}}.jpg", config.path, id_);
  spdlog::debug("FrameLoggerBase<{}>: base_template_ = {}", id_,
                base_template_);
}

void FrameLoggerBase::Run() {
  seq_template_ = fmt::format(base_template_, state_.sequence++);
  spdlog::debug("FrameLoggerBase<{}>: seq_template_ = {}", id_, seq_template_);

  client_logger_.Info(fmt::format("FrameLoggerBase<{}>: first log {}", id_,
                                  GetFrameImagePath(1)));

  RunLoop();

  client_logger_.Info(fmt::format("FrameLoggerBase<{}>: last log {}", id_,
                                  GetFrameImagePath(frame_count_ - 1)));
}

bool FrameLoggerBase::CheckMount(const FrameLogConfig& config) {
  struct stat mnt {};
  struct stat parent {};

  // check mount point
  if (stat(config.path.c_str(), &mnt)) {
    spdlog::error("FrameLoggerBase<{}>: failed to stat {}: {}", id_,
                  config.path, std::strerror(errno));
    return false;
  }

  // ...and its parent
  std::string parent_path = config.path + "/..";
  if (stat(parent_path.c_str(), &parent)) {
    spdlog::error("FrameLoggerBase<{}>: failed to stat {}: {}", id_,
                  parent_path, std::strerror(errno));
    return false;
  }

  // compare st_dev fields, if equal then both belong to same filesystem
  bool mounted = mnt.st_dev != parent.st_dev;
  if (mounted == config.mount) {
    spdlog::debug("FrameLoggerBase<{}>: {} is a mounted filesystem", id_,
                  config.path);
    return true;
  } else {
    spdlog::error(
        "FrameLoggerBase<{}>: {} has mounted filesystem is {}, expected {}",
        id_, config.path, mounted, config.mount);
    return false;
  }
}

bool FrameLoggerBase::CheckDir(const FrameLogConfig& config) {
  // verify base path is dir
  DIR* dir = opendir(config.path.c_str());
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    spdlog::error("FrameLoggerBase<{}>: {} does not exist", id_, config.path);
    return false;
  } else {
    spdlog::error("FrameLoggerBase<{}>: failed to opendir {}: {}", id_,
                  config.path, std::strerror(errno));
    return false;
  }

  std::string path = fmt::format("{}/{}", config.path, id_);
  dir = opendir(path.c_str());
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    spdlog::info("FrameLoggerBase<{}>: making directory {}", id_, path);
    if (mkdir(path.c_str(), 0777)) {
      spdlog::error("FrameLoggerBase<{}>: failed to mkdir {}: {}", id_, path,
                    std::strerror(errno));
      return false;
    }
  } else {
    spdlog::error("FrameLoggerBase<{}>: failed to opendir {}: {}", id_, path,
                  std::strerror(errno));
    return false;
  }
  return true;
}
