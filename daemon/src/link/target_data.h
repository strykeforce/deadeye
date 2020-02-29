#pragma once
#include <nlohmann/json.hpp>
#include <opencv2/core/mat.hpp>

using json = nlohmann::json;

namespace deadeye {
struct TargetData {
  static char const* kIdKey;
  static char const* kSerialKey;
  static char const* kValidKey;

  std::string id;
  int serial;
  bool valid;

  TargetData() = default;
  virtual ~TargetData() = default;
  TargetData(std::string id, int serial, bool valid);

  TargetData(TargetData const&) = delete;
  TargetData& operator=(TargetData const&) = delete;

  virtual void DrawMarkers(cv::Mat& preview);
  virtual std::string Dump() const;  // json format
  virtual std::string ToString() const { return Dump(); }
};
}  // namespace deadeye
