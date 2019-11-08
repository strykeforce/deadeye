#include "pipeline/pipeline_logger.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>
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
      cancel_(cancel) {}

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
