import io from "socket.io-client";

// const socket = io("http://" + document.domain + ":" + window.location.port);
const socket = io("http://" + document.domain + ":" + 5000);

socket.on("connect_error", (error) => {
  console.error(error);
});

socket.on("error", (error) => {
  console.error(error);
});

export const subscribeToUnitUpdates = (handleUnitsChange) => {
  socket.on("refresh", (data) => handleUnitsChange(data));
};

export const subscribeToLinkUpdates = (handleLinksChange) => {
  socket.on("link", (data) => handleLinksChange(data));
};

export const unsubscribeFromLinkUpdates = () => {
  socket.off("link");
};

export const close = () => {
  socket.close();
  console.debug("closed socket.io connection");
};

export const refreshLink = () => socket.emit("link_refresh", "pls");

export const sendMessage = (msg) => socket.emit("message", msg);

export const enableCamera = (unit, inum, enabled) => {
  const message = { unit, inum, enabled };
  socket.emit("camera_control", message);
};

export const enableLight = (unit, inum, enabled) => {
  const message = { unit, inum, enabled };
  socket.emit("light_control", message);
};

export const configCapture = (unit, inum, capture) => {
  const message = { unit, inum, capture };
  socket.emit("capture_config", message);
  // console.log(`configCapture: ${JSON.stringify(message)}`);
};

export const configPipeline = (unit, inum, pipeline) => {
  pipeline.sn++;
  const message = { unit, inum, pipeline };
  socket.emit("pipeline_config", message);
  // console.log(`configPipeline: ${JSON.stringify(message)}`);
};

export const configStream = (unit, inum, stream) => {
  stream.sn++;
  const message = { unit, inum, stream };
  socket.emit("stream_config", message);
  // console.log(`configStream: ${JSON.stringify(message)}`);
};

export const configImageUpload = (unit, inum, upload) => {
  const message = { unit, inum, image: upload };
  socket.emit("image_upload", message);
  // console.log(`configImageUpload: ${JSON.stringify(message)}`);
};

export const configLink = (link) => {
  const message = { link };
  socket.emit("link_config", message);
  console.log(`configLink: ${JSON.stringify(message)}`);
};
