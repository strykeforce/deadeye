#include "capture/image_file.h"

#include <fmt/core.h>

#include <chrono>
#include <nlohmann/json.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <thread>

using namespace deadeye;
using json = nlohmann::json;

ImageFile::ImageFile(const CaptureConfig& config)
    : size_{config.width, config.height}, fps_{config.frame_rate} {
  json j = config.config;
  if (!j.is_object()) j = json::object();

  auto image_path = j.value("image", "");
  LoadSource(image_path);
}

bool ImageFile::Grab(cv::Mat& frame) {
  std::chrono::duration<double> fp_s{1.0 / fps_};
  std::this_thread::sleep_for(fp_s);
  frame = src_;
  return true;
}

void ImageFile::LoadInvalidSource(const std::string& path) {
  src_ = cv::Mat::zeros(size_, CV_8UC3);
  auto sep = path.rfind("/");
  auto file = sep != std::string::npos ? path.substr(sep + 1) : path;
  std::string text = fmt::format("ERROR LOADING: {}", file);
  int scale = size_.width / 320;
  auto position = cv::Point(0, size_.height / 2);
  cv::putText(src_, text, position, cv::FONT_HERSHEY_PLAIN, scale,
              cv::Scalar::all(255), scale);
}

void ImageFile::LoadSource(const std::string& path) {
  auto image = cv::imread(path);
  if (image.empty()) {
    LoadInvalidSource(path);
    return;
  }

  auto inter = image.rows > size_.height ? cv::INTER_AREA : cv::INTER_CUBIC;
  if (image.size() != size_) {
    cv::resize(image, src_, size_, 0, 0, inter);
    return;
  }
  src_ = image;
}