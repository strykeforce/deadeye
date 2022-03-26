// Copyright (c) 2022 Stryke Force FRC 2767
#pragma once

#include <nlohmann/json.hpp>
#include <utility>
#include <vector>

namespace deadeye {
struct FrameLoggerState {
  static constexpr auto kFrameSeq = "sequence";
  unsigned int sequence{1};

  FrameLoggerState() = default;
  explicit FrameLoggerState(unsigned int sequence);
};

void to_json(nlohmann::json& j, const FrameLoggerState& fls);
void from_json(const nlohmann::json& j, FrameLoggerState& fls);

struct PipelineState {
  static constexpr auto kFrameLogger = "frame";

  FrameLoggerState frame_logger;
  int inum_;

  PipelineState() = default;
  explicit PipelineState(int inum);
  explicit PipelineState(int inum, FrameLoggerState frame_logger);

  void Store();
  static PipelineState Load(int inum);
};

void to_json(nlohmann::json& j, const PipelineState& pls);
void from_json(const nlohmann::json& j, PipelineState& pls);

}  // namespace deadeye
