#include "abstract_pipeline.h"

#include "config/deadeye_config.h"
#include "pipeline/pipeline_ops.h"

using namespace deadeye;

AbstractPipeline::AbstractPipeline(int inum)
    : Pipeline{inum}, id_(DEADEYE_UNIT + std::to_string(inum)) {}

static int gcd(int a, int b) { return (b == 0) ? a : gcd(b, a % b); }

/**
 * ConfigCapture handles changes to capture settings and only takes effect
 * after pipeline restart.
 */
void AbstractPipeline::ConfigCapture(CaptureConfig const &config) {
  capture_config_ = config;
  pipeline_type_ = capture_config_.PipelineType();
  // fps_ = capture_config_.frame_rate;
  int h = capture_config_.OutputSize().height;
  int w = capture_config_.OutputSize().width;
  preview_resize_ = w != kStreamSize.width;

  int r = gcd(w, h);
  int wr = w / r;
  int hr = h / r;
  spdlog::debug("{}: capture output: {}x{} ({}:{})", *this, w, h, wr, hr);

  if (wr == 4 && hr == 3) {  // 4:3
    preview_border_ = 0;
  } else if (wr == 16 && hr == 9) {
    preview_border_ = h / 6;
  } else {
    spdlog::error("{}: invalid aspect ratio: {}:{}", *this, wr, hr);
    preview_border_ = 0;
  }

  spdlog::debug("{}: {} {}", *this, capture_config_,
                preview_resize_ ? "[preview resized]" : "");
}

/**
 * ConfigPipeline handles changes to pipeline config.
 */
void AbstractPipeline::ConfigPipeline(PipelineConfig const &config) {
  pipeline_config_ = config;
}

/**
 * ConfigStream handles changes to video streaming.
 */
void AbstractPipeline::ConfigStream(StreamConfig const &config) {
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
TargetDataPtr AbstractPipeline::ProcessFrame(cv::Mat const &frame) {
  frame_ = frame;
  MaskFrame(frame_, hsv_threshold_output_, pipeline_config_.HsvLow(),
            pipeline_config_.HsvHigh());
  FindContours(hsv_threshold_output_, find_contours_output_);

  FilterContours(pipeline_config_.filter, find_contours_output_,
                 filter_contours_output_);
  TargetDataPtr target_data = ProcessTarget(filter_contours_output_);
  return std::move(target_data);
}

void AbstractPipeline::ProcessStreamFrame(cv::Mat &preview,
                                          TargetData const *target_data) {
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
