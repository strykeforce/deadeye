#pragma once
#include <opencv2/core/types.hpp>
#include <string>

#define DE_STRINGIFY(s) DE_STR(s)
#define DE_STR(s) #s

#define DEADEYE_UNIT DE_STRINGIFY(DEADEYE_UNIT_ID)

#define DE_DEADEYE_TABLE "/Deadeye"
#define DE_CAPTURE "/Capture"
#define DE_CONFIG "/Config"
#define DE_INFO "/Info"
#define DE_LIGHTS "/Light"
#define DE_LINK "/Link"
#define DE_PIPELINE "/Pipeline"
#define DE_STREAM "/Stream"
#define DE_CONTROL_TABLE DE_DEADEYE_TABLE "/" DEADEYE_UNIT
#define DE_CONFIG_TABLE DE_DEADEYE_TABLE "/" DEADEYE_UNIT

#define DE_CAMERA_CONTROL_TABLE(inum) DE_CONTROL_TABLE "/" inum
#define DE_LIGHTS_CONTROL_TABLE(inum) DE_CAMERA_CONTROL_TABLE(inum) DE_LIGHTS

#define DE_ERROR "Error"
#define DE_ON "On"
#define DE_OFF "Off"
#define DE_BLINK "Blink"
#define DE_ADDRESS "Address"
#define DE_PORT "Port"

#define DE_CAMERA_CONTROL(inum, param) DE_CAMERA_CONTROL_TABLE(inum) "/" param
#define DE_LIGHTS_CONTROL(inum, param) DE_LIGHTS_CONTROL_TABLE(inum) "/" param
#define DE_CAPTURE_CONFIG_ENTRY(inum) DE_CONFIG_TABLE "/" inum DE_CAPTURE
#define DE_PIPELINE_CONFIG_ENTRY(inum) DE_CONFIG_TABLE "/" inum DE_PIPELINE
#define DE_STREAM_CONFIG_ENTRY(inum) DE_CONFIG_TABLE "/" inum DE_STREAM
#define DE_INFO_ENTRY(inum) DE_CONFIG_TABLE "/" inum DE_INFO
#define DE_LINK_ENTRY DE_DEADEYE_TABLE DE_LINK

#define CLIENT_PORT DEADEYE_CLIENT_PORT
#define CLIENT_ADDRESS DE_STRINGIFY(DEADEYE_CLIENT_ADDRESS)
#define NT_SERVER DE_STRINGIFY(DEADEYE_NT_SERVER)

namespace deadeye {
std::string CameraId(int inum);
std::string CameraControlTablePath(int inum);
std::string LightsControlTablePath(int inum);
std::string CaptureConfigEntryPath(int inum);
std::string PipelineConfigEntryPath(int inum);
std::string StreamConfigEntryPath(int inum);
std::string InfoEntryPath(int inum);

static const cv::Size kStreamSize{320, 240};
}  // namespace deadeye
