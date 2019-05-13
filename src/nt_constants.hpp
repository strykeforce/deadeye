#pragma once

#include <hedley.h>

#define DE_DEADEYE_TABLE "/Deadeye"
#define DE_CONTROL_TABLE DE_DEADEYE_TABLE "/Control"
#define DE_CONFIG_TABLE DE_DEADEYE_TABLE "/Config"

#define DE_CAMERA_CONTROL_TABLE(inum) DE_CONTROL_TABLE "/Camera" inum

#define DE_ENABLED "Enabled"
#define DE_ERROR "Error"
#define DE_LIGHTS "Lights"

#define DE_CAMERA_CONTROL(inum, param) DE_CAMERA_CONTROL_TABLE(inum) "/" param
