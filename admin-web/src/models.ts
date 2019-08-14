export interface Units {
  [unit: string]: Unit;
}

export interface Unit {
  id: string;
  cameras: Cameras;
}

export interface Cameras {
  [inum: string]: Camera;
}

export interface Camera {
  id: string;
  inum: number;
  unit: string;
  on: boolean;
  error: boolean;
  streamUrl: string;
  config: CameraConfig;
}

export interface CameraConfig {
  exposure: number;
  hue: number[];
  sat: number[];
  sn: number;
  val: number[];
}
