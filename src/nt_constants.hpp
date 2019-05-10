#pragma once

#include <hedley.h>

#define DE_DEADEYE_TABLE "/Deadeye"
#define DE_CONTROL_TABLE DE_DEADEYE_TABLE "/Control"
#define DE_CONFIG_TABLE DE_DEADEYE_TABLE "/Config"

#define DE_CAMERA0_CONTROL_TABLE DE_CONTROL_TABLE "/Camera0"

#define DE_ENABLED "Enabled"
#define DE_LIGHTS "Lights"

#define DE_CAMERA0_CONTROL(param) DE_CAMERA0_CONTROL_TABLE "/" param
