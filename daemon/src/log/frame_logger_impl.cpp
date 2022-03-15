#include "log/frame_logger_impl.h"

#include <dirent.h>
#include <sys/stat.h>

#include <utility>

using namespace deadeye::logger;

int FrameLoggerImpl::enable_count_{0};

FrameLoggerImpl::FrameLoggerImpl(std::string id, const FrameLogConfig& config,
                                 LoggerQueue& queue, std::atomic<bool>& cancel)
    : id_{std::move(std::move(id))},
      enabled_{config.fps > 0 && CheckMount(config) && CheckDir(config)},
      queue_(queue),
      cancel_{cancel} {
  FrameLoggerImpl::enable_count_++;
  template_ = fmt::format("{}/{{}}/{}-{{}}.jpg", config.path,
                          FrameLoggerImpl::enable_count_);
}

bool FrameLoggerImpl::CheckMount(const FrameLogConfig& config) {
  struct stat mnt {};
  struct stat parent {};

  // check mount point
  if (stat(config.path.c_str(), &mnt)) {
    spdlog::error("LoggerImpl<{}>: failed to stat {}: {}", id_, config.path,
                  std::strerror(errno));
    return false;
  }

  // ...and its parent
  std::string parent_path = config.path + "/..";
  if (stat(parent_path.c_str(), &parent)) {
    spdlog::error("LoggerImpl<{}>: failed to stat {}: {}", id_, parent_path,
                  std::strerror(errno));
    return false;
  }

  // compare st_dev fields, if equal then both belong to same filesystem
  bool mounted = mnt.st_dev != parent.st_dev;
  if (mounted == config.mount) {
    spdlog::debug("LoggerImpl<{}>: {} is a mounted filesystem", id_,
                  config.path);
    return true;
  } else {
    spdlog::error(
        "LoggerImpl<{}>: {} has mounted filesystem is {}, expected {}", id_,
        config.path, mounted, config.mount);
    return false;
  }
}

bool FrameLoggerImpl::CheckDir(const FrameLogConfig& config) {
  // verify base path is dir
  DIR* dir = opendir(config.path.c_str());
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    spdlog::error("LoggerImpl<{}>: {} does not exist", id_, config.path);
    return false;
  } else {
    spdlog::error("LoggerImpl<{}>: failed to opendir {}: {}", id_, config.path,
                  std::strerror(errno));
    return false;
  }

  std::string path = fmt::format("{}/{}", config.path, id_);
  dir = opendir(path.c_str());
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    spdlog::info("LoggerImpl<{}>: making directory {}", id_, path);
    if (mkdir(path.c_str(), 0777)) {
      spdlog::error("LoggerImpl<{}>: failed to mkdir {}: {}", id_, path,
                    std::strerror(errno));
      return false;
    }
  } else {
    spdlog::error("LoggerImpl<{}>: failed to opendir {}: {}", id_, path,
                  std::strerror(errno));
    return false;
  }
  return true;
}