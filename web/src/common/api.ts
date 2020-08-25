import io from "socket.io-client";
import { CaptureConfig, PipelineConfig, StreamConfig, Link } from "./models";

const socket = io("http://" + document.domain + ":" + window.location.port);

socket.on("connect_error", (error: object) => {
  console.error(error);
});

socket.on("error", (error: object) => {
  console.error(error);
});

export const subscribeToUnitUpdates = (
  handleUnitsChange: (units: string) => void
): void => {
  socket.on("refresh", (data: string) => handleUnitsChange(data));
};

export const subscribeToLinkUpdates = (
  handleLinksChange: (units: string) => void
): void => {
  socket.on("link", (data: string) => handleLinksChange(data));
};

export const unsubscribeFromLinkUpdates = () => {
  socket.off("link");
};

export const close = (): void => {
  socket.close();
  console.log("closed IO socket");
};

export const refreshLink = () => socket.emit("link_refresh", "pls");

export const sendMessage = (msg: string) => socket.emit("message", msg);

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

export const configCapture = (
  unit: string,
  inum: number,
  capture: CaptureConfig
): void => {
  const message = { unit, inum, capture };
  socket.emit("capture_config", message);
  // console.log(`configCapture: ${JSON.stringify(message)}`);
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

export const configImageUpload = (
  unit: string,
  inum: number,
  upload: string
): void => {
  const message = { unit, inum, image: upload };
  socket.emit("image_upload", message);
  // console.log(`configImageUpload: ${JSON.stringify(message)}`);
};

export const configLink = (link: Link[]) => {
  const message = { link };
  socket.emit("link_config", message);
  console.log(`configLink: ${JSON.stringify(message)}`);
};
