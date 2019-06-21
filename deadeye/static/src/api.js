import io from "socket.io-client";

var socket;

export const subscribe = handleUnitsChange => {
  socket = io("http://" + document.domain + ":" + window.location.port);
  socket.on("refresh", data => handleUnitsChange(data));
};

export const close = () => {
  socket.close();
};

export const enableCamera = (unit, inum, enabled) => {
  const message = { unit, inum, enabled };
  socket.emit("camera_control", message);
};

export const configCamera = (unit, inum, config) => {
  const message = { unit, inum, config };
  socket.emit("camera_config", message);
  // console.log(`configCamera: ${JSON.stringify(message)}`);
};
