// Copyright (c) 2022 Stryke Force FRC 2767

#include "pipeline/streamer.h"

#include <wpi/Logger.h>

#include <map>
#include <opencv2/imgproc.hpp>

#include "pipeline/pipeline.h"

using ::deadeye::Streamer;

namespace {
void ResetCScoreLogging();
#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
int gcd(int a, int b) { return (b == 0) ? a : gcd(b, a % b); }
#pragma clang diagnostic pop
}  // namespace

Streamer::Streamer(const Pipeline* pipeline, const cv::Size size)
    : pipeline_{pipeline},
      server_{"cvhttpserver", 5805 + pipeline_->GetInum()},
      source_{"cvsource", cs::VideoMode::kMJPEG, kStreamSize.width,
              kStreamSize.height, 30} {
  ResetCScoreLogging();
  int h = size.height;
  int w = size.width;
  resize_ = w != kStreamSize.width;

  int r = gcd(w, h);
  int wr = w / r;
  int hr = h / r;
  spdlog::debug("Streamer: capture output: {}x{} ({}:{})", w, h, wr, hr);

  tb_border_ = 0;
  lr_border_ = 0;
  frame_tb_border_ = 2;
  frame_lr_border_ = 2;
  if (wr == 4 && hr == 3) {  // 4:3
    // no change
  } else if (wr == 16 && hr == 9) {
    tb_border_ = h / 6;
  } else if (wr == 9 && hr == 16) {
    lr_border_ = w * 2 / 3;
    frame_tb_border_ = 1;
  } else {
    spdlog::error("Streamer: invalid aspect ratio: {}:{}", wr, hr);
    tb_border_ = 0;
  }

  spdlog::debug("Streamer tb_border: {}, lr_border_ {} {}", tb_border_,
                lr_border_, resize_ ? "[output resized]" : "");

  server_.SetSource(source_);
  spdlog::info("{} streaming on port {}", *pipeline_, server_.GetPort());
}

void Streamer::Configure(const StreamConfig& config) { config_ = config; }

void Streamer::Process(const cv::Mat& frame, const TargetData* target) {
  using View = StreamConfig::View;
  using Contour = StreamConfig::Contour;

  switch (config_.view) {
    case View::none:
      if (config_.contour != Contour::none) {
        output_ = cv::Mat::zeros(frame.size(), CV_8UC3);
      }
      cv::copyMakeBorder(output_, output_, frame_tb_border_, frame_tb_border_,
                         frame_lr_border_, frame_lr_border_,
                         cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
      break;
    case View::original:
      if (config_.contour != Contour::none) {
        cv::cvtColor(frame, output_, cv::COLOR_BGR2GRAY);
        cv::cvtColor(output_, output_, cv::COLOR_GRAY2BGR);
        break;
      }
      output_ = frame;
      break;
    case View::mask:
      cv::cvtColor(pipeline_->GetMask(), output_, cv::COLOR_GRAY2BGR);
      cv::copyMakeBorder(output_, output_, frame_tb_border_, frame_tb_border_,
                         frame_lr_border_, frame_lr_border_,
                         cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
      break;
  }

  switch (config_.contour) {
    case Contour::none:
      break;
    case Contour::filter:
      cv::drawContours(output_, pipeline_->GetFilteredContours(), -1,
                       cv::Scalar(255, 0, 240), 2);
      target->DrawMarkers(output_);
      break;

    case Contour::all:
      cv::drawContours(output_, pipeline_->GetContours(), -1,
                       cv::Scalar(255, 0, 240), 2);
      break;
  }

  if (tb_border_ != 0)
    cv::copyMakeBorder(output_, output_, tb_border_, tb_border_, 0, 0,
                       cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

  if (lr_border_ != 0)
    cv::copyMakeBorder(output_, output_, 0, 0, lr_border_, lr_border_,
                       cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

  if (resize_) cv::resize(output_, output_, kStreamSize, 0, 0, cv::INTER_AREA);

  source_.PutFrame(output_);
}

namespace {
void ResetCScoreLogging() {
  static std::map<unsigned int, spdlog::level::level_enum> levels{
      {wpi::WPI_LOG_DEBUG4, spdlog::level::debug},
      {wpi::WPI_LOG_DEBUG3, spdlog::level::debug},
      {wpi::WPI_LOG_DEBUG2, spdlog::level::debug},
      {wpi::WPI_LOG_DEBUG1, spdlog::level::debug},
      {wpi::WPI_LOG_DEBUG, spdlog::level::debug},
      {wpi::WPI_LOG_INFO, spdlog::level::info},
      {wpi::WPI_LOG_WARNING, spdlog::level::warn},
      {wpi::WPI_LOG_ERROR, spdlog::level::err},
      {wpi::WPI_LOG_CRITICAL, spdlog::level::critical}};

  cs::SetLogger(
      [](unsigned int level, char const* file, unsigned int line,
         char const* msg) {
        spdlog::log(levels[level], "cscore: {} in {}, line {}", msg, file,
                    line);
      },
      wpi::WPI_LOG_WARNING);
}

}  // namespace
