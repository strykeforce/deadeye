#pragma once
#include <nlohmann/json.hpp>

#include "link/target_data.hpp"

using json = nlohmann::json;

namespace deadeye {
struct CenterTargetData : public TargetData {
  static char const* kXKey;
  static char const* kYKey;

  int x;
  int y;

  CenterTargetData();
  virtual ~CenterTargetData() = default;
  CenterTargetData(std::string id, int sn, bool valid, int x, int y);

  virtual void DrawMarkers(cv::Mat& preview) override;
  virtual std::string Dump() const override;
};

void to_json(json& j, CenterTargetData const& td);
void from_json(json const& j, CenterTargetData& td);

}  // namespace deadeye
