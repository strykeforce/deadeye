#include "pipeline/pipeline_logger.h"

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
  // disable logging if mounted log filesystem expected but missing
  enabled_ = enabled_ && CheckMount(config);
}

void PipelineLogger::operator()() {
  int i = 0;
  PipelineLogEntry entry;
  if (enabled_)
    spdlog::info("Logging pipeline to {}", fmt::format(template_, id_, "nnn"));
  else
    spdlog::warn("Pipeline logging disabled");

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
      spdlog::error("Exception writing frame: {}", ex.what());
    }
    spdlog::trace("wrote image to {}", path);

    if (queue_.size_approx() > 0)
      spdlog::warn("frame logging queue: {}", queue_.size_approx());
  }
  spdlog::trace("Frame logging task exited");
}

bool PipelineLogger::CheckMount(LogConfig const& config) {
  struct stat mnt;
  struct stat parent;

  // check mount point
  if (stat(config.path.c_str(), &mnt) == -1) {
    spdlog::error("failed to stat {}: {}", config.path, std::strerror(errno));
    return false;
  }

  // ...and its parent
  std::string parent_path = config.path + "/..";
  if (stat(parent_path.c_str(), &parent) == -1) {
    spdlog::error("failed to stat {}: {}", parent_path, std::strerror(errno));
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
