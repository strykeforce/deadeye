export type Id = string | undefined;

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
  info: CameraInfo;
}

export interface CameraInfo {
  pipeline: { name: string };
  version: string;
}

export interface CaptureConfig {
  type: string;
  w: number;
  h: number;
  fps: number;
  config: object;
}

export interface FilterConfig {
  area: [number, number];
  aspect: [number, number];
  solidity: [number, number];
}

export interface LogConfig {
  path: string;
  enabled: boolean;
  mount: boolean;
}

export interface PipelineConfig {
  sn: number;
  hue: [number, number];
  sat: [number, number];
  val: [number, number];
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
  view: "none" | "original" | "mask";
  contour: "none" | "filter" | "all";
}

export interface PanelProps {
  units: Units;
  selectedId: string;
}
