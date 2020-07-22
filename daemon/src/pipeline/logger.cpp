#include "pipeline/logger.h"

#include <dirent.h>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>

#include <cstring>
#include <nlohmann/json.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "pipeline/pipeline_ops.h"

using namespace deadeye;
using json = nlohmann::json;

namespace {
static const cv::Size kFrameSize{640, 360};
}

int Logger::enable_count_{0};

LogEntry::LogEntry(cv::Mat const frame, Contours filtered_contours,
                   TargetDataPtr target)
    : frame(frame.clone()),
      filtered_contours(filtered_contours),
      target(std::move(target)) {}

Logger::Logger(std::string id, CaptureConfig capture_config,
               PipelineConfig pipeline_config, LoggerQueue& queue,
               std::atomic<bool>& cancel)
    : id_(id),
      enabled_(pipeline_config.log.fps > 0 && CheckMount(pipeline_config.log) &&
               CheckDir(pipeline_config.log)),
      capture_(capture_config),
      hsv_low_(pipeline_config.HsvLow()),
      hsv_high_(pipeline_config.HsvHigh()),
      filter_(pipeline_config.filter),

      queue_(queue),
      cancel_(cancel) {
  // disable logging if filesystem checks fail
  Logger::enable_count_++;
  template_ = fmt::format("{}/{{}}/{}-{{}}.jpg", pipeline_config.log.path,
                          Logger::enable_count_);
}

void Logger::operator()() {
  int seq = 1;
  LogEntry entry;
  if (enabled_)
    spdlog::info("Logger<{}>: logging to {}", id_,
                 fmt::format(template_, id_, "nnn"));
  else
    spdlog::warn("Logger<{}>: logging disabled", id_);

  while (!cancel_.load()) {
    if (!queue_.wait_dequeue_timed(entry, std::chrono::milliseconds(100))) {
      continue;
    }
    if (!enabled_) continue;  // throw away if logged by upstream while disabled

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - begin_);

    cv::Mat mask;
    MaskFrame(entry.frame, mask, hsv_low_, hsv_high_);
    Contours contours;
    FindContours(mask, contours);

    auto path = fmt::format(template_, id_, seq);
    try {
      if (entry.frame.cols > kFrameSize.width) {
        cv::resize(entry.frame, entry.frame, kFrameSize, 0, 0, cv::INTER_AREA);
        cv::resize(mask, mask, kFrameSize, 0, 0, cv::INTER_AREA);
      }

      cv::cvtColor(mask, mask, cv::COLOR_GRAY2BGR);
      cv::Mat mat_array[] = {entry.frame, mask, cv::Mat()};

      cv::Mat top;
      cv::hconcat(mat_array, 2, top);

      cv::Mat gray;
      cv::cvtColor(entry.frame, gray, cv::COLOR_BGR2GRAY);
      cv::cvtColor(gray, gray, cv::COLOR_GRAY2BGR);
      cv::drawContours(gray, entry.filtered_contours, -1,
                       cv::Scalar(255, 0, 240), 2);
      entry.target->DrawMarkers(gray);

      cv::Mat black{entry.frame.size(), CV_8UC3, cv::Scalar::all(0)};
      cv::drawContours(black, contours, -1, cv::Scalar(255, 0, 240), 2);

      mat_array[0] = black;
      mat_array[1] = gray;
      cv::Mat bottom;
      cv::hconcat(mat_array, 2, bottom);

      cv::Mat output;
      cv::Mat info{top.rows / 4, top.cols, CV_8UC3, cv::Scalar::all(255)};

      std::string text = fmt::format(
          "CAPTURE: exp={} cap={}x{} out={}x{} seq={} elapsed={} msec",
          capture_.exposure, capture_.capture_width, capture_.capture_height,
          capture_.output_width, capture_.output_height, seq, elapsed.count());
      int font = cv::FONT_HERSHEY_PLAIN;
      double font_scale = 1;
      int thickness = 1;
      int baseline = 0;
      // cv::getTextSize(text, font, font_scale, thickness, &baseline);
      baseline = info.rows / 4;

      cv::Point text_org{2, baseline - 5};

      cv::putText(info, text, text_org, font, font_scale, cv::Scalar::all(0),
                  thickness, cv::LINE_8);
      text_org += cv::Point(0, baseline);

      text = fmt::format(
          "PIPELINE: hue=[{:.0f}, {:.0f}] sat=[{:.0f}, {:.0f}] "
          "val=[{:.0f}, {:.0f}] area=[{:.2f}, {:.2f}], solidity=[{:.2f}, "
          "{:.2f}], aspect=[{:.2f}, {:.2f}], contours={}/{}",
          hsv_low_[0], hsv_high_[0], hsv_low_[1], hsv_high_[1], hsv_low_[2],
          hsv_high_[2], filter_.area[0], filter_.area[1], filter_.solidity[0],
          filter_.solidity[1], filter_.aspect[0], filter_.aspect[1],
          entry.filtered_contours.size(), contours.size());

      cv::putText(info, text, text_org, font, font_scale, cv::Scalar::all(0),
                  thickness, cv::LINE_8);
      text_org += cv::Point(0, baseline);

      text = fmt::format("TARGET: {}", entry.target->ToString());

      cv::putText(info, text, text_org, font, font_scale, cv::Scalar::all(0),
                  thickness, cv::LINE_8);

      mat_array[0] = top;
      mat_array[1] = bottom;
      mat_array[2] = info;
      cv::vconcat(mat_array, 3, output);

      cv::imwrite(path, output);
    } catch (const cv::Exception& ex) {
      spdlog::error("Logger<{}>: write exception: {}", id_, ex.what());
    }
    spdlog::trace("Logger<{}>: wrote image to {}", id_, path);

    if (queue_.size_approx() > 0)
      spdlog::warn("Logger<{}>: queue filling: {}", id_, queue_.size_approx());

    seq++;
  }
  spdlog::trace("Logger<{}>: task exited", id_);
}

bool Logger::CheckMount(const LogConfig& config) {
  struct stat mnt;
  struct stat parent;

  // check mount point
  if (stat(config.path.c_str(), &mnt)) {
    spdlog::error("Logger<{}>: failed to stat {}: {}", id_, config.path,
                  std::strerror(errno));
    return false;
  }

  // ...and its parent
  std::string parent_path = config.path + "/..";
  if (stat(parent_path.c_str(), &parent)) {
    spdlog::error("Logger<{}>: failed to stat {}: {}", id_, parent_path,
                  std::strerror(errno));
    return false;
  }

  // compare st_dev fields, if equal then both belong to same filesystem
  bool mounted = mnt.st_dev != parent.st_dev;
  if (mounted == config.mount) {
    spdlog::debug("Logger<{}>: {} is a mounted filesystem", id_, config.path);
    return true;
  } else {
    spdlog::error("Logger<{}>: {} has mounted filesystem is {}, expected {}",
                  id_, config.path, mounted, config.mount);
    return false;
  }
}

bool Logger::CheckDir(const LogConfig& config) {
  // verify base path is dir
  DIR* dir = opendir(config.path.c_str());
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    spdlog::error("Logger<{}>: {} does not exist", id_, config.path);
    return false;
  } else {
    spdlog::error("Logger<{}>: failed to opendir {}: {}", id_, config.path,
                  std::strerror(errno));
    return false;
  }

  std::string path = fmt::format("{}/{}", config.path, id_);
  dir = opendir(path.c_str());
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    spdlog::info("Logger<{}>: making directory {}", id_, path);
    if (mkdir(path.c_str(), 0777)) {
      spdlog::error("Logger<{}>: failed to mkdir {}: {}", id_, path,
                    std::strerror(errno));
      return false;
    }
  } else {
    spdlog::error("Logger<{}>: failed to opendir {}: {}", id_, path,
                  std::strerror(errno));
    return false;
  }
  return true;
}
