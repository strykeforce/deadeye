// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <fmt/core.h>
#include <networktables/NetworkTableValue.h>
#include <spdlog/fmt/ostr.h>

#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>
#include <string>

#include "capture/capture_type.h"

namespace deadeye {

struct CaptureConfig {
  static constexpr auto kTypeKey = "type";
  static constexpr auto kWidthKey = "w";
  static constexpr auto kHeightKey = "h";
  static constexpr auto kFrameRateKey = "fps";
  static constexpr auto kConfigKey = "config";

  CaptureType type = CaptureType::test;
  int width = 0;
  int height = 0;
  int frame_rate = 0;
  nlohmann::json config = nlohmann::json::object();

  /**
   * Default constructor.
   */
  CaptureConfig() = default;

  /**
   * Constructor from member values.
   */
  CaptureConfig(CaptureType type, int width, int height, int frame_rate,
                nlohmann::json config);

  /**
   * Constructor from NetworkTables.
   */
  explicit CaptureConfig(const std::shared_ptr<nt::Value>& value);

  /*
   * Get output size of frame.
   */
  [[nodiscard]] cv::Size Size() const;

  /*
   * Get string representation of capture type.
   */
  [[nodiscard]] std::string PipelineType() const;

  template <typename OStream>
  friend OStream& operator<<(OStream& os, const CaptureConfig& cc) {
    std::string output =
        fmt::format("CaptureConfig<type={}, w={}, h={}, fps={}, config={}",
                    cc.PipelineType(), cc.width, cc.height, cc.frame_rate,
                    cc.config.dump());
    os << output;
    return os;
  }
};

void to_json(nlohmann::json& j, const CaptureConfig& cc);
void from_json(const nlohmann::json& j, CaptureConfig& cc);

inline bool operator==(const CaptureConfig& lhs, const CaptureConfig& rhs) {
  return lhs.type == rhs.type && lhs.width == rhs.width &&
         lhs.height == rhs.height && lhs.frame_rate == rhs.frame_rate &&
         lhs.config == rhs.config;
}

inline bool operator!=(const CaptureConfig& lhs, const CaptureConfig& rhs) {
  return !(lhs == rhs);
}

}  // namespace deadeye
