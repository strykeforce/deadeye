#pragma once

#define DE_DEADEYE_TABLE "/Deadeye"
#define DE_CAMERA "/Camera"
#define DE_LIGHTS "/Lights"
#define DE_CONTROL_TABLE DE_DEADEYE_TABLE "/Control"
#define DE_CONFIG_TABLE DE_DEADEYE_TABLE "/Config"

#define DE_CAMERA_CONTROL_TABLE(inum) DE_CONTROL_TABLE DE_CAMERA inum
#define DE_LIGHTS_CONTROL_TABLE(inum) DE_CAMERA_CONTROL_TABLE(inum) DE_LIGHTS

#define DE_ERROR "Error"
#define DE_ON "On"
#define DE_OFF "Off"
#define DE_BLINK "Blink"

#define DE_CAMERA_CONTROL(inum, param) DE_CAMERA_CONTROL_TABLE(inum) "/" param
#define DE_LIGHTS_CONTROL(inum, param) DE_LIGHTS_CONTROL_TABLE(inum) "/" param
