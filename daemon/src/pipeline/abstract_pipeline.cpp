#include "abstract_pipeline.h"

#include "config/deadeye_config.h"
#include "pipeline/pipeline_ops.h"

using namespace deadeye;

AbstractPipeline::AbstractPipeline(int inum)
    : Pipeline{inum}, id_(DEADEYE_UNIT + std::to_string(inum)) {}

static int gcd(int a, int b) { return (b == 0) ? a : gcd(b, a % b); }

namespace {
std::pair<bool, int> StreamOutputFormat(const CaptureConfig& config) {
  int h = config.OutputSize().height;
  int w = config.OutputSize().width;
  bool resize = w != kStreamSize.width;

  int r = gcd(w, h);
  int wr = w / r;
  int hr = h / r;
  spdlog::debug("StreamOutputFormat: capture output: {}x{} ({}:{})", w, h, wr,
                hr);

  int border;
  if (wr == 4 && hr == 3) {  // 4:3
    int border = 0;
  } else if (wr == 16 && hr == 9) {
    border = h / 6;
  } else {
    spdlog::error("StreamOutputFormat: invalid aspect ratio: {}:{}", wr, hr);
    border = 0;
  }

  spdlog::debug("StreamOutputFormat: {} {}", config,
                resize ? "[preview resized]" : "");

  return std::pair<bool, int>(resize, border);
}
}  // namespace

/**
 * Configure handles changes to capture settings and only takes effect
 * after pipeline restart.
 */
void AbstractPipeline::Configure(const CaptureConfig& config) {
  capture_config_ = config;

  center_ = cv::Point{config.output_width / 2, config.output_height / 2};
  center2f_ = static_cast<cv::Point2f>(center_);
  frame_area_ = config.output_height * config.output_width;

  auto of = StreamOutputFormat(capture_config_);
  preview_resize_ = of.first;
  preview_border_ = of.second;
}

/**
 * Configure handles changes to pipeline config.
 */
void AbstractPipeline::Configure(PipelineConfig const& config) {
  pipeline_config_ = config;
}

/**
 * Configure handles changes to video streaming.
 */
void AbstractPipeline::Configure(StreamConfig const& config) {
  stream_config_ = config;
}

/**
 * Get a copy of all found contours.
 */
Contours AbstractPipeline::GetContours() { return find_contours_output_; }

/**
 * Get a copy of the filtered contours.
 */
Contours AbstractPipeline::GetFilteredContours() {
  return filter_contours_output_;
}

/**
 * Process a frame and return target data.
 */
TargetDataPtr AbstractPipeline::ProcessFrame(cv::Mat const& frame) {
  frame_ = frame;
  MaskFrame(frame_, hsv_threshold_output_, pipeline_config_.HsvLow(),
            pipeline_config_.HsvHigh());
  FindContours(hsv_threshold_output_, find_contours_output_);

  FilterContours(pipeline_config_.filter, find_contours_output_,
                 filter_contours_output_);
  TargetDataPtr target_data = ProcessTarget(filter_contours_output_);
  return std::move(target_data);
}

void AbstractPipeline::FilterContours(FilterConfig const& filter,
                                      Contours const& src, Contours& dest) {
  GeometricContoursFilter(filter, src, dest);
}

TargetDataPtr AbstractPipeline::ProcessTarget(Contours const& contours) {
  return std::make_unique<TargetData>(id_, 0, false);
}

void AbstractPipeline::ProcessStreamFrame(cv::Mat& preview,
                                          TargetData const* target_data) {
  using View = StreamConfig::View;
  using Contour = StreamConfig::Contour;

  switch (stream_config_.view) {
    case View::none:
      if (stream_config_.contour != Contour::none) {
        preview = cv::Mat::zeros(frame_.size(), CV_8UC3);
      }
      break;
    case View::original:
      if (stream_config_.contour != Contour::none) {
        cv::cvtColor(frame_, preview, cv::COLOR_BGR2GRAY);
        cv::cvtColor(preview, preview, cv::COLOR_GRAY2BGR);
        break;
      }
      preview = frame_;
      break;
    case View::mask:
      cv::cvtColor(hsv_threshold_output_, preview, cv::COLOR_GRAY2BGR);
      break;
  }

  switch (stream_config_.contour) {
    case Contour::none:
      break;
    case Contour::filter:
      cv::drawContours(preview, filter_contours_output_, -1,
                       cv::Scalar(255, 0, 240), 2);
      target_data->DrawMarkers(preview);
      break;

    case Contour::all:
      cv::drawContours(preview, find_contours_output_, -1,
                       cv::Scalar(255, 0, 240), 2);
      break;
  }

  if (preview_border_ != 0) {
    cv::copyMakeBorder(preview, preview, preview_border_, preview_border_, 0, 0,
                       cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
  }

  if (preview_resize_)
    cv::resize(preview, preview, kStreamSize, 0, 0, cv::INTER_AREA);
}

std::string AbstractPipeline::ToString() const {
  assert(pipeline_type_ != "");
  return fmt::format("AbstractPipeline<{}, {}>", id_, pipeline_type_);
}