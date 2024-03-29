// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <nlohmann/json.hpp>
#include <opencv2/core/mat.hpp>
#include <string>

using json = nlohmann::json;

namespace deadeye {
struct TargetData {
  static const char* kIdKey;
  static const char* kSerialKey;
  static const char* kValidKey;
  static const char* kDataKey;

  std::string_view id;
  int serial{};
  bool valid{};

  TargetData() = default;
  virtual ~TargetData() = default;
  TargetData(std::string_view id, int serial, bool valid);

  TargetData(const TargetData&) = delete;
  TargetData& operator=(const TargetData&) = delete;

  virtual void DrawMarkers(cv::Mat& preview) const;
  [[nodiscard]] virtual std::string Dump() const;  // json format
  [[nodiscard]] virtual std::string ToString() const { return Dump(); }
};
}  // namespace deadeye
