#pragma once

#include <atomic>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>

#include "pipeline.hpp"

namespace deadeye {
struct SteamConfig;
struct PipelineConfig;

class AbstractPipeline : public Pipeline {
 public:
  AbstractPipeline(int inum);
  virtual ~AbstractPipeline();
  void UpdateConfig(PipelineConfig *config) override;
  void UpdateStream(StreamConfig *config) override;
  virtual void Run() override;
  virtual void CancelTask() override;

 protected:
  virtual cv::VideoCapture GetVideoCapture() = 0;
  inline virtual cv::Mat PreProcessFrame(cv::Mat const &frame) { return frame; }
  virtual void ProcessFrame(cv::Mat const &frame) = 0;
  virtual void FilterContours(std::vector<std::vector<cv::Point>> const &src,
                              std::vector<std::vector<cv::Point>> &dest) = 0;

  std::atomic<PipelineConfig *> pipeline_config_{nullptr};
  std::atomic<StreamConfig *> stream_config_{nullptr};
  std::atomic<bool> cancel_{false};

 private:
  void LogTickMeter(cv::TickMeter tm);

  StreamConfig *prev_stream_config_{nullptr};
  PipelineConfig *prev_pipeline_config_{nullptr};
};

}  // namespace deadeye
