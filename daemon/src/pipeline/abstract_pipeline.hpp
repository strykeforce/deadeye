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
  void CancelTask() override;
  virtual void Run() override;

 protected:
  virtual cv::VideoCapture GetVideoCapture();
  virtual cv::Mat PreProcessFrame(cv::Mat const &frame);
  virtual void ProcessFrame(cv::Mat const &frame);
  virtual void FilterContours(std::vector<std::vector<cv::Point>> const &src,
                              std::vector<std::vector<cv::Point>> &dest);

  std::atomic<PipelineConfig *> pipeline_config_{nullptr};
  std::atomic<StreamConfig *> stream_config_{nullptr};
  std::atomic<bool> cancel_{false};

 private:
  void LogTickMeter(cv::TickMeter tm);

  StreamConfig *prev_stream_config_{nullptr};
  PipelineConfig *prev_pipeline_config_{nullptr};
};

}  // namespace deadeye
