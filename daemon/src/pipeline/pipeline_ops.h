#pragma once

#include <opencv2/imgproc.hpp>

#define DE_IN_RANGE(frame, low, high, output)       \
  {                                                 \
    cv::cvtColor(frame, output, cv::COLOR_BGR2HSV); \
    cv::inRange(output, (low), (high), output);     \
  }

#define DE_FIND_CONTOURS(mask, contours)                \
  {                                                     \
    contours.clear();                                   \
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, \
                     cv::CHAIN_APPROX_SIMPLE);          \
  }
