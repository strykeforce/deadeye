Pipeline
========

protected: inum_

- GetInum
- ConfigCapture
- ConfigPipeline
- ConfigStream
- StartCapture
- StopCapture
- GrabFrame
- ProcessFrame
- GetContours
- GetFilteredContours
- ProcessStreamFrame
- ToString

AbstractPipeline
================

public: id_, pipeline_type_
protected: capture_config_, pipeline_config_, stream_config_
private: frame_, hsv_threshold_output_, find_contours_output_, filter_contours_output_, preview_border_, preview_resize_

- ConfigCapture - capture_config_, pipeline_type_, preview_resize_, preview_border_
- ConfigPipeline - pipeline_config_
- ConfigStream - stream_config_
- GetContours - find_contours_output_
- GetFilteredContours - filter_contours_output_
- ProcessFrame - frame_, hsv_threshold_output_, pipeline_config_, find_contours_output_, filter_contours_output_
- ProcessStreamFrame - stream_config_, frame_, hsv_threshold_output_, filter_contours_output_, find_contours_output_, preview_border_, preview_resize_

DefaultPipeline
===============

protected: cap_, center_, center2f_, frame_area_

- StartCapture - cap_
- StopCapture - cap_
- GrabFrame - cap_
- FilterContours
- ProcessTarget - id_
- ToString - id_, pipeline_type_

UprightRectPipeline
===================

- ProcessTarget - id_, center_
- ToString - id_, pipeline_type_