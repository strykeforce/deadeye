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
  stream: StreamConfig;
  config: CameraConfig;
}

export interface CameraConfig {
  sn: number;
  exposure: number;
  hue: number[];
  sat: number[];
  val: number[];
}

export interface StreamConfig {
  sn: number;
  url: string;
}
