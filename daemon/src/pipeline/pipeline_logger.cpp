#include "pipeline/pipeline_logger.h"

#include <dirent.h>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <cstring>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace deadeye;

PipelineLogEntry::PipelineLogEntry(cv::Mat const frame, cv::Mat const mask,
                                   Contours contours,
                                   Contours filtered_contours,
                                   TargetDataPtr target)
    : frame(frame.clone()),
      mask(mask.clone()),
      contours(contours),
      filtered_contours(filtered_contours),
      target(std::move(target)) {}

PipelineLogger::PipelineLogger(std::string id, LogConfig config,
                               PipelineLoggerQueue& queue,
                               std::atomic<bool>& cancel)
    : id_(id),
      enabled_(config.enabled),
      template_(fmt::format("{}/{{}}/{{}}.jpg", config.path)),
      queue_(queue),
      cancel_(cancel) {
  // disable logging if filesystem checks fail
  enabled_ = enabled_ && CheckMount(config);
  enabled_ = enabled_ && CheckDir(config);
}

void PipelineLogger::operator()() {
  int i = 0;
  PipelineLogEntry entry;
  if (enabled_)
    spdlog::info("PipelineLogger<{}>: logging to {}", id_,
                 fmt::format(template_, id_, "nnn"));
  else
    spdlog::warn("PipelineLogger<{}>: logging disabled", id_);

  while (!cancel_.load()) {
    if (!queue_.wait_dequeue_timed(entry, std::chrono::milliseconds(100))) {
      continue;
    }
    if (!enabled_) continue;  // throw away if logged by upstream while disabled

    auto path = fmt::format(template_, id_, i++);
    try {
      cv::cvtColor(entry.mask, entry.mask, cv::COLOR_GRAY2BGR);
      cv::Mat mat_array[] = {entry.frame, entry.mask};

      cv::Mat top;
      cv::hconcat(mat_array, 2, top);

      cv::Mat gray;
      cv::cvtColor(entry.frame, gray, cv::COLOR_BGR2GRAY);
      cv::cvtColor(gray, gray, cv::COLOR_GRAY2BGR);
      cv::drawContours(gray, entry.filtered_contours, -1,
                       cv::Scalar(255, 0, 240), 2);
      entry.target->DrawMarkers(gray);

      cv::Mat black = cv::Mat::zeros(entry.frame.size(), CV_8UC3);
      cv::drawContours(black, entry.contours, -1, cv::Scalar(255, 0, 240), 2);

      mat_array[0] = black;
      mat_array[1] = gray;
      cv::Mat bottom;
      cv::hconcat(mat_array, 2, bottom);

      cv::Mat output;
      mat_array[0] = top;
      mat_array[1] = bottom;
      cv::vconcat(mat_array, 2, output);

      cv::imwrite(path, output);
    } catch (const cv::Exception& ex) {
      spdlog::error("PipelineLogger<{}>: write exception: {}", id_, ex.what());
    }
    spdlog::trace("PipelineLogger<{}>: wrote image to {}", id_, path);

    if (queue_.size_approx() > 0)
      spdlog::warn("PipelineLogger<{}>: queue filling: {}", id_,
                   queue_.size_approx());
  }
  spdlog::trace("PipelineLogger<{}>: task exited", id_);
}

bool PipelineLogger::CheckMount(LogConfig const& config) {
  struct stat mnt;
  struct stat parent;

  // check mount point
  if (stat(config.path.c_str(), &mnt)) {
    spdlog::error("PipelineLogger<{}>: failed to stat {}: {}", id_, config.path,
                  std::strerror(errno));
    return false;
  }

  // ...and its parent
  std::string parent_path = config.path + "/..";
  if (stat(parent_path.c_str(), &parent)) {
    spdlog::error("PipelineLogger<{}>: failed to stat {}: {}", id_, parent_path,
                  std::strerror(errno));
    return false;
  }

  // compare st_dev fields, if equal then both belong to same filesystem
  bool mounted = mnt.st_dev != parent.st_dev;
  if (mounted == config.mount) {
    spdlog::debug("PipelineLogger<{}>: {} is a mounted filesystem", id_,
                  config.path);
    return true;
  } else {
    spdlog::error(
        "PipelineLogger<{}>: {} has mounted filesystem is {}, expected {}", id_,
        config.path, mounted, config.mount);
    return false;
  }
}

bool PipelineLogger::CheckDir(LogConfig const& config) {
  // verify base path is dir
  DIR* dir = opendir(config.path.c_str());
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    spdlog::error("PipelineLogger<{}>: {} does not exist", id_, config.path);
    return false;
  } else {
    spdlog::error("PipelineLogger<{}>: failed to opendir {}: {}", id_,
                  config.path, std::strerror(errno));
    return false;
  }

  std::string path = fmt::format("{}/{}", config.path, id_);
  dir = opendir(path.c_str());
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    spdlog::info("PipelineLogger<{}>: making directory {}", id_, path);
    if (mkdir(path.c_str(), 0777)) {
      spdlog::error("PipelineLogger<{}>: failed to mkdir {}: {}", id_, path,
                    std::strerror(errno));
      return false;
    }
  } else {
    spdlog::error("PipelineLogger<{}>: failed to opendir {}: {}", id_, path,
                  std::strerror(errno));
    return false;
  }
  return true;
}
