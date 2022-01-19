#include "abstract_pipeline.h"

#include <utility>

#include "config/deadeye_config.h"
#include "pipeline/pipeline_ops.h"

using namespace deadeye;

AbstractPipeline::AbstractPipeline(int inum, std::string name)
    : Pipeline{inum, std::move(name)}, id_(DEADEYE_UNIT + std::to_string(inum)) {}

/**
 * Configure handles changes to capture settings and only takes effect
 * after pipeline restart.
 */
void AbstractPipeline::Configure(const CaptureConfig& config) {
  capture_type_ = config.PipelineType();
}

/**
 * Configure handles changes to pipeline config.
 */
void AbstractPipeline::Configure(PipelineConfig const& config) {
  pipeline_config_ = config;
}

/**
 * Process a frame and return target data.
 */
std::unique_ptr<TargetData> AbstractPipeline::ProcessFrame(
    cv::Mat const& frame) {
  frame_ = frame;
  MaskFrame(frame_, hsv_threshold_output_, pipeline_config_.HsvLow(),
            pipeline_config_.HsvHigh());
  FindContours(hsv_threshold_output_, find_contours_output_);

  FilterContours(pipeline_config_.filter, find_contours_output_,
                 filter_contours_output_);
  std::unique_ptr<TargetData> target_data =
      ProcessContours(filter_contours_output_);
  return std::move(target_data);
}

void AbstractPipeline::FilterContours(FilterConfig const& filter,
                                      Contours const& src, Contours& dest) {
  GeometricContoursFilter(filter, src, dest);
}

std::unique_ptr<TargetData> AbstractPipeline::ProcessContours(
    Contours const& contours) {
  return std::make_unique<TargetData>(id_, 0, false);
}

std::string AbstractPipeline::ToString() const {
  return fmt::format("AbstractPipeline<{}, {}>", id_, capture_type_);
}