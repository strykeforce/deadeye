#pragma once
#include <nlohmann/json.hpp>
#include <opencv2/core/mat.hpp>

using json = nlohmann::json;

namespace deadeye {
struct TargetData {
  static const char* kIdKey;
  static const char* kSerialKey;
  static const char* kValidKey;
  static const char* kDataKey;

  std::string id;
  int serial{};
  bool valid{};

  TargetData() = default;
  virtual ~TargetData() = default;
  TargetData(std::string id, int serial, bool valid);

  TargetData(const TargetData&) = delete;
  TargetData& operator=(const TargetData&) = delete;

  virtual void DrawMarkers(cv::Mat& preview) const;
  [[nodiscard]] virtual std::string Dump() const;  // json format
  [[nodiscard]] virtual std::string ToString() const { return Dump(); }
};
}  // namespace deadeye
