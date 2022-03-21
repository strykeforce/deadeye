#include "state.h"

#include <fstream>
#include <iostream>

using namespace deadeye;
using json = nlohmann::json;

// FrameLoggerState
FrameLoggerState::FrameLoggerState(unsigned int sequence)
    : sequence(sequence) {}

void deadeye::to_json(json& j, const FrameLoggerState& fls) {
  j = json{{FrameLoggerState::kFrameSeq, fls.sequence}};
}

void deadeye::from_json(const json& j, FrameLoggerState& fls) {
  j.at(FrameLoggerState::kFrameSeq).get_to(fls.sequence);
}

// PipelineState
PipelineState::PipelineState(FrameLoggerState frame_logger)
    : frame_logger(frame_logger){};

void deadeye::to_json(json& j, const PipelineState& pls) {
  j = json{{PipelineState::kFrameLogger, pls.frame_logger}};
}

void deadeye::from_json(const json& j, PipelineState& pls) {
  j.at(PipelineState::kFrameLogger).get_to(pls.frame_logger);
}

// State
State::State(std::vector<PipelineState> pipelines)
    : pipelines(std::move(pipelines)){};

void deadeye::to_json(json& j, const State& s) {
  j = json{{State::kPipelines, s.pipelines}};
}

void deadeye::from_json(const json& j, State& s) {
  j.at(State::kPipelines).get_to(s.pipelines);
}

void State::Store(std::ostream& os) const {
  json j = *this;
  os << j;
}

State State::Load(std::istream& is) {
  json j;
  is >> j;
  return j;
}
