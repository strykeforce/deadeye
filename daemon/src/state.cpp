#include "state.h"

#include <networktables/NetworkTableInstance.h>

#include "config.h"

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
PipelineState::PipelineState(int inum) : inum_(inum), frame_logger() {}
PipelineState::PipelineState(int inum, FrameLoggerState frame_logger)
    : inum_(inum), frame_logger(frame_logger){};

void deadeye::to_json(json& j, const PipelineState& pls) {
  j = json{{PipelineState::kFrameLogger, pls.frame_logger}};
}

void deadeye::from_json(const json& j, PipelineState& pls) {
  j.at(PipelineState::kFrameLogger).get_to(pls.frame_logger);
}

namespace {
nt::NetworkTableEntry get_entry(int inum) {
  nt::NetworkTableInstance nti = nt::NetworkTableInstance::GetDefault();
  return nti.GetEntry(StateEntryPath(inum));
}

constexpr std::string_view kDefault = "DEFAULT";
}  // namespace

void PipelineState::Store() {
  auto entry = get_entry(inum_);
  json j = *this;
  entry.SetString(j.dump());
  entry.SetPersistent();
}

PipelineState PipelineState::Load(int inum) {
  std::string entry = get_entry(inum).GetString(kDefault);
  if (entry == kDefault) return {inum};
  PipelineState state = json::parse(entry);
  state.inum_ = inum;
  return state;
}
