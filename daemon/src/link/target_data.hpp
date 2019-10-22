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

  TargetData();
  virtual ~TargetData() = default;
  TargetData(std::string id, int serial, bool valid);
  virtual void DrawMarkers(cv::Mat& preview);
  virtual std::string Dump() const;
};

void to_json(json& j, TargetData const& td);
void from_json(json const& j, TargetData& td);

}  // namespace deadeye
