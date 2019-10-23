#pragma once

#include <atomic>

namespace deadeye {

struct StreamConfig;

class DefaultStreamer {
 public:
  DefaultStreamer(/* args */);
  ~DefaultStreamer();
  void UpdateStream(StreamConfig *config);

 private:
  std::atomic<StreamConfig *> stream_config_{nullptr};
  StreamConfig *prev_stream_config_{nullptr};
};

DefaultStreamer::DefaultStreamer(/* args */) {}

DefaultStreamer::~DefaultStreamer() {}

}  // namespace deadeye
