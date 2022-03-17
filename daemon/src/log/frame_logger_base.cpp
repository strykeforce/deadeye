#include "log/frame_logger_base.h"

#include <dirent.h>
#include <sys/stat.h>

#include <utility>

using namespace deadeye::logger;

int FrameLoggerBase::sequence_{0};

FrameLoggerBase::FrameLoggerBase(const int inum, const FrameLogConfig& config,
                                 FrameLoggerQueue& queue,
                                 std::atomic<bool>& cancel)
    : id_{CameraId(inum)},
      config_{config},
      enabled_{config.fps > 0 && CheckMount(config) && CheckDir(config)},
      queue_(queue),
      cancel_{cancel},
      client_logger{inum} {
  FrameLoggerBase::sequence_++;
  template_ = fmt::format("{}/{{}}/{}-{{}}.jpg", config.path,
                          FrameLoggerBase::sequence_);
}

void FrameLoggerBase::Run() {
  if (enabled_)
    client_logger.Info(fmt::format("FrameLogger<{}>: logging to " + template_,
                                   id_, id_, "nnn"));
  else
    client_logger.Warn(fmt::format("FullFrame<{}>: logging disabled", id_));
  RunLoop();
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
