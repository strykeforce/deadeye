#include "log/four_up.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <cstring>
#include <nlohmann/json.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "pipeline/pipeline_ops.h"

using namespace deadeye::logger;
using json = nlohmann::json;

namespace {
static const cv::Size kFrameSize{640, 360};
}

FourUp::FourUp(std::string id, CaptureConfig capture_config,
               PipelineConfig pipeline_config, LogConfig log_config,
               LoggerQueue& queue, std::atomic<bool>& cancel)
    : LoggerImpl(id, log_config, queue, cancel),
      capture_(capture_config),
      hsv_low_(pipeline_config.HsvLow()),
      hsv_high_(pipeline_config.HsvHigh()),
      filter_(pipeline_config.filter) {
  // disable logging if filesystem checks fail
}

void FourUp::Run() {
  int seq = 1;
  LogEntry entry;
  if (enabled_)
    spdlog::info("FourUp<{}>: logging to {}", id_,
                 fmt::format(template_, id_, "nnn"));
  else
    spdlog::warn("FourUp<{}>: logging disabled", id_);

  begin_ = std::chrono::high_resolution_clock::now();

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
      cv::cvtColor(mask, mask, cv::COLOR_GRAY2BGR);

      cv::Mat vrule{entry.frame.rows, 1, CV_8UC3, cv::Scalar::all(255)};
      // three mats concatenated horizontally, leave space for four mats
      // concatenated vertically below
      cv::Mat mat_array[] = {entry.frame, vrule, mask, cv::Mat()};

      cv::Mat top;
      cv::hconcat(mat_array, 3, top);

      cv::Mat gray;
      cv::cvtColor(entry.frame, gray, cv::COLOR_BGR2GRAY);
      cv::cvtColor(gray, gray, cv::COLOR_GRAY2BGR);
      cv::drawContours(gray, entry.filtered_contours, -1,
                       cv::Scalar(255, 0, 240), 2);
      entry.target->DrawMarkers(gray);

      cv::Mat black{entry.frame.size(), CV_8UC3, cv::Scalar::all(0)};
      cv::drawContours(black, contours, -1, cv::Scalar(255, 0, 240), 2);

      mat_array[0] = black;
      mat_array[2] = gray;
      cv::Mat bottom;
      cv::hconcat(mat_array, 3, bottom);

      cv::Mat output;
      // info panel prints well at this size, scale up or down as needed
      // for frame size.
      cv::Mat info{90, 1280, CV_8UC3, cv::Scalar::all(255)};

      std::string text = fmt::format(
          "CAPTURE: exp={} cap={}x{} out={}x{} seq={} elapsed={} msec", 0, 0, 0,
          capture_.width, capture_.height, seq, elapsed.count());
      int font = cv::FONT_HERSHEY_PLAIN;
      double font_scale = 1;
      int thickness = 1;
      int baseline = 0;
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

      cv::Size info_size{top.cols, static_cast<int>(std::round(top.rows / 4))};
      cv::resize(
          info, info, info_size,
          info.rows < info_size.height ? cv::INTER_CUBIC : cv::INTER_AREA);

      cv::Mat hrule{1, top.cols, CV_8UC3, cv::Scalar::all(255)};
      mat_array[0] = top;
      mat_array[1] = hrule;
      mat_array[2] = bottom;
      mat_array[3] = info;
      cv::vconcat(mat_array, 4, output);

      cv::imwrite(path, output);
    } catch (const cv::Exception& ex) {
      spdlog::error("FourUp<{}>: write exception: {}", id_, ex.what());
    }
    spdlog::trace("FourUp<{}>: wrote image to {}", id_, path);

    if (queue_.size_approx() > 0)
      spdlog::warn("FourUp<{}>: queue filling: {}", id_, queue_.size_approx());

    seq++;
  }
  spdlog::debug("FourUp<{}>: task exited", id_);
}