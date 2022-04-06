// Copyright (c) 2022 Stryke Force FRC 2767

#include "state.h"

#include <networktables/NetworkTableInstance.h>
#include <spdlog/spdlog.h>

#include <string>

#include "config.h"

using ::deadeye::FrameLoggerState;
using ::deadeye::PipelineState;
using ::deadeye::StateEntryPath;
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
    : inum_(inum), frame_logger(frame_logger) {}

void deadeye::to_json(json& j, const PipelineState& pls) {
  j = json{{PipelineState::kFrameLogger, pls.frame_logger}};
}

void deadeye::from_json(const json& j, PipelineState& pls) {
  j.at(PipelineState::kFrameLogger).get_to(pls.frame_logger);
}

namespace {
nt::NetworkTableEntry get_entry(int inum) {
  nt::NetworkTableInstance nti = nt::NetworkTableInstance::GetDefault();
  auto entry = nti.GetEntry(StateEntryPath(inum));
  entry.SetPersistent();
  return entry;
}

constexpr std::string_view kDefault = "DEFAULT";
}  // namespace

void PipelineState::Store() {
  auto entry = get_entry(inum_);
  json j = *this;
  auto j_str = j.dump();
  entry.SetString(j_str);
  spdlog::debug("PipelineState<{}> stored: {}", inum_, j_str);
}

PipelineState PipelineState::Load(int inum) {
  std::string entry = get_entry(inum).GetString(kDefault);
  if (entry == kDefault) return PipelineState{inum};
  PipelineState state = json::parse(entry);
  state.inum_ = inum;
  return state;
}
