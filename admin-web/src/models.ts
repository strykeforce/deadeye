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
  light: Light;
  capture: CaptureConfig;
  pipeline: PipelineConfig;
  stream: StreamConfig;
}

export interface CaptureConfig {
  exp: number;
  type: string;
  ch: number;
  cw: number;
  oh: number;
  ow: number;
  flip: number;
  fps: number;
}

export interface FilterConfig {
  area: number[];
  aspect: number[];
  solidity: number[];
}

export interface LogConfig {
  path: string;
  enabled: boolean;
  mount: boolean;
}

export interface PipelineConfig {
  sn: number;
  exposure: number;
  hue: number[];
  sat: number[];
  val: number[];
  filter: FilterConfig;
  log: LogConfig;
}

export interface Light {
  on: boolean;
  blink: boolean;
}

export interface StreamConfig {
  sn: number;
  url: string;
  view: 'none' | 'original' | 'mask';
  contour: 'none' | 'filter' | 'all';
}

export interface PanelProps {
  units: Units;
  selectedId: string;
}
