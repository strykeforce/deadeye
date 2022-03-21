#pragma once

#include <nlohmann/json.hpp>
#include <vector>

namespace deadeye {
struct FrameLoggerState {
  static constexpr auto kFrameSeq = "sequence";
  int sequence{0};

  explicit FrameLoggerState(const nlohmann::json& j);
};

void to_json(nlohmann::json& j, const FrameLoggerState& s);
void from_json(const nlohmann::json& j, FrameLoggerState& s);

struct PipelineState {
  static constexpr auto kFrameLogger = "frame";

  FrameLoggerState frame_logger;
};

struct State {
  static constexpr auto kPipelines = "pipelines";

  std::vector<PipelineState> pipelines;

  explicit State(const nlohmann::json& j);

  static State Load(std::string_view path);

};

void to_json(nlohmann::json& j, const State& s);
void from_json(const nlohmann::json& j, State& s);

}  // namespace deadeye
