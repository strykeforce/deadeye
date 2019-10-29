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
  stream: StreamConfig;
  pipeline: PipelineConfig;
}

export interface PipelineConfig {
  sn: number;
  exposure: number;
  hue: number[];
  sat: number[];
  val: number[];
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
