#include "default_pipeline.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <opencv2/imgproc.hpp>

#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "link/center_target_data.h"

using namespace deadeye;

DefaultPipeline::DefaultPipeline(int inum) : AbstractPipeline{inum} {}

bool DefaultPipeline::StartCapture() {
  assert(capture_config_ready_.load());
  if (cap_.isOpened()) return true;

  safe::ReadAccess<LockableCaptureConfig> cc{capture_config_};
  auto pipeline = cc->Pipeline();
  spdlog::debug("{}: {}", *this, pipeline);
  center_ = cv::Point{cc->output_width / 2, cc->output_height / 2};
  return cap_.open(pipeline, cv::CAP_GSTREAMER);
}

void DefaultPipeline::StopCapture() { cap_.release(); }

bool DefaultPipeline::GrabFrame(cv::Mat &frame) {
  if (!cap_.read(frame)) return false;

  // FIXME: pipeline should prefer 720p aspect ratio
  int rows = frame.rows;
  if (rows == 120 || rows == 240 || rows == 480 || rows == 960) return true;

  // change aspect ration
  int border = frame.rows / 6;
  cv::copyMakeBorder(frame, frame, border, border, 0, 0, cv::BORDER_CONSTANT,
                     cv::Scalar(0, 0, 0));
  return true;
}

// This filter returns the contour with the largest area.
void DefaultPipeline::FilterContours(Contours const &src, Contours &dest) {
  dest.clear();
  auto max_area_iter = std::max_element(
      src.begin(), src.end(),
      [](std::vector<cv::Point> const &a, std::vector<cv::Point> const &b) {
        return cv::contourArea(a) < cv::contourArea(b);
      });
  if (max_area_iter != src.end()) dest.push_back(*max_area_iter);

  // throw PipelineException("Test Exception");
}

// Target is center of contour bounding box.
TargetDataPtr DefaultPipeline::ProcessTarget(Contours const &contours) {
  if (contours.size() == 0)
    return std::make_unique<CenterTargetData>(id_, 0, false, 0, 0);
  auto contour = contours[0];
  auto bb = cv::boundingRect(contour);
  cv::Point target = (bb.tl() + bb.br()) / 2;
  cv::Point offset = target - center_;
  return std::make_unique<CenterTargetData>(id_, 0, true, offset.x, offset.y);
}

std::string DefaultPipeline::ToString() const {
  assert(pipeline_type_ != "");
  return fmt::format("DefaultPipeline<{}, {}>", id_, pipeline_type_);
}
