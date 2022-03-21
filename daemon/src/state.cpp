#include "state.h"

#include <fstream>

using namespace deadeye;
using json = nlohmann::json;

FrameLoggerState::FrameLoggerState(const nlohmann::json& j) {
  //
}

State::State(const nlohmann::json& j) {
  j.at(State::kInitialFrameSeq).get_to(initial_frame_sequence);
}

State State::Load(std::string_view path) {
  std::ifstream ifs{path};
  return State(json::parse(ifs));
}

// ---------------------------------------------------------------------------
// nlohmann_json support
//
void deadeye::to_json(json& j, const State& s) {
  j = json{{State::kInitialFrameSeq, s.initial_frame_sequence}};
}

void deadeye::from_json(const json& j, State& l) {
  j.at(State::kInitialFrameSeq).get_to(l.initial_frame_sequence);
}