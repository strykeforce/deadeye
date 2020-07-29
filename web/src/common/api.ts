import io from "socket.io-client";
import { PipelineConfig, StreamConfig } from "./models";

let socket: SocketIOClient.Socket;

export const subscribe = (handleUnitsChange: (units: string) => void): void => {
  socket = io("http://" + document.domain + ":" + window.location.port);
  socket.on("refresh", (data: string) => handleUnitsChange(data));
};

export const close = (): void => {
  socket.close();
};

export const enableCamera = (
  unit: string,
  inum: number,
  enabled: boolean
): void => {
  const message = { unit, inum, enabled };
  socket.emit("camera_control", message);
};

export const enableLight = (
  unit: string,
  inum: number,
  enabled: boolean
): void => {
  const message = { unit, inum, enabled };
  socket.emit("light_control", message);
};

export const configPipeline = (
  unit: string,
  inum: number,
  pipeline: PipelineConfig
): void => {
  pipeline.sn++;
  const message = { unit, inum, pipeline };
  socket.emit("pipeline_config", message);
  // console.log(`configPipeline: ${JSON.stringify(message)}`);
};

export const configStream = (
  unit: string,
  inum: number,
  stream: StreamConfig
): void => {
  stream.sn++;
  const message = { unit, inum, stream };
  socket.emit("stream_config", message);
  // console.log(`configStream: ${JSON.stringify(message)}`);
};
