#include "default_pipeline.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <opencv2/imgproc.hpp>

#include "config/pipeline_config.h"
#include "config/stream_config.h"
#include "link/upright_target_data.h"

using namespace deadeye;

DefaultPipeline::DefaultPipeline(int inum) : AbstractPipeline{inum} {}

bool DefaultPipeline::StartCapture() {
  if (cap_.isOpened()) return true;

  auto pipeline = capture_config_.Pipeline();
  spdlog::debug("{}: {}", *this, pipeline);
  center_ = cv::Point{capture_config_.output_width / 2,
                      capture_config_.output_height / 2};
  center2f_ = static_cast<cv::Point2f>(center_);
  frame_area_ = capture_config_.output_height * capture_config_.output_width;
  return cap_.open(pipeline, cv::CAP_GSTREAMER);
}

void DefaultPipeline::StopCapture() { cap_.release(); }

bool DefaultPipeline::GrabFrame(cv::Mat &frame) { return cap_.read(frame); }

// Filter with FilterConfig or max area contour
void DefaultPipeline::FilterContours(FilterConfig const &filter,
                                     Contours const &src, Contours &dest) {
  dest.clear();

  if (filter.enabled) {
    for (auto const &contour : src) {
      // set these to true if filter is skipped, false otherwise
      bool area_ok = !filter.area_enabled;
      bool fullness_ok = !filter.fullness_enabled;
      bool aspect_ok = !filter.aspect_enabled;

      double area = cv::contourArea(contour);
      cv::Rect bb = cv::boundingRect(contour);

      if (filter.area_enabled) {
        double ratio = bb.area() / frame_area_;
        area_ok = ratio >= filter.area[0] && ratio <= filter.area[1];
        // spdlog::debug("area = {}", ratio);
      }

      if (filter.fullness_enabled) {
        std::vector<cv::Point> hull;
        cv::convexHull(contour, hull);
        double hull_area = cv::contourArea(hull);
        double fullness = area / hull_area;
        fullness_ok =
            fullness >= filter.fullness[0] && fullness <= filter.fullness[1];
        // spdlog::debug("fullness = {}", fullness);
      }

      if (filter.aspect_enabled) {
        double aspect =
            static_cast<double>(bb.width) / static_cast<double>(bb.height);
        aspect_ok = aspect >= filter.aspect[0] && aspect <= filter.aspect[1];
        // spdlog::debug("aspect = {}", aspect);
      }

      if (area_ok && fullness_ok && aspect_ok) {
        dest.push_back(contour);
      }
    }

    return;
  }

  // if no filters configured, return max area contour
  auto max_area_iter = std::max_element(
      src.begin(), src.end(),
      [](std::vector<cv::Point> const &a, std::vector<cv::Point> const &b) {
        return cv::contourArea(a) < cv::contourArea(b);
      });
  if (max_area_iter != src.end()) dest.push_back(*max_area_iter);

  // throw PipelineException("Test Exception");
}

std::string DefaultPipeline::ToString() const {
  assert(pipeline_type_ != "");
  return fmt::format("DefaultPipeline<{}, {}>", id_, pipeline_type_);
}
