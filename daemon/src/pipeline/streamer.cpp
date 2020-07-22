#include "pipeline/streamer.h"

#include <wpi/Logger.h>

#include <opencv2/imgproc.hpp>

using namespace deadeye;

namespace {
void ResetCScoreLogging();
static int gcd(int a, int b) { return (b == 0) ? a : gcd(b, a % b); }
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

  if (wr == 4 && hr == 3) {  // 4:3
    border_ = 0;
  } else if (wr == 16 && hr == 9) {
    border_ = h / 6;
  } else {
    spdlog::error("Streamer: invalid aspect ratio: {}:{}", wr, hr);
    border_ = 0;
  }

  spdlog::debug("Streamer border: {} {}", border_,
                resize_ ? "[output resized]" : "");

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

  if (border_ != 0)
    cv::copyMakeBorder(output_, output_, border_, border_, 0, 0,
                       cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

  if (resize_) cv::resize(output_, output_, kStreamSize, 0, 0, cv::INTER_AREA);

  source_.PutFrame(output_);
}

namespace {
void ResetCScoreLogging() {
  using namespace wpi;
  using namespace spdlog;
  static std::map<unsigned int, level::level_enum> levels{
      {WPI_LOG_DEBUG4, level::debug},     {WPI_LOG_DEBUG3, level::debug},
      {WPI_LOG_DEBUG2, level::debug},     {WPI_LOG_DEBUG1, level::debug},
      {WPI_LOG_DEBUG, level::debug},      {WPI_LOG_INFO, level::info},
      {WPI_LOG_WARNING, level::warn},     {WPI_LOG_ERROR, level::err},
      {WPI_LOG_CRITICAL, level::critical}};

  cs::SetLogger(
      [](unsigned int level, char const* file, unsigned int line,
         char const* msg) {
        spdlog::log(levels[level], "cscore: {} in {}, line {}", msg, file,
                    line);
      },
      WPI_LOG_WARNING);
}

}  // namespace
