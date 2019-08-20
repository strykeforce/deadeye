import io from 'socket.io-client';
import { CameraConfig } from './models';

let socket: SocketIOClient.Socket;

export const subscribe = (handleUnitsChange: (units: string) => void): void => {
  socket = io('http://' + document.domain + ':' + window.location.port);
  socket.on('refresh', (data: string) => handleUnitsChange(data));
};

export const close = (): void => {
  socket.close();
};

export const enableCamera = (unit: string, inum: number, enabled: boolean): void => {
  const message = { unit, inum, enabled };
  socket.emit('camera_control', message);
};

export const enableLight = (unit: string, inum: number, enabled: boolean): void => {
  const message = { unit, inum, enabled };
  socket.emit('light_control', message);
};

export const configCamera = (unit: string, inum: number, config: CameraConfig): void => {
  const message = { unit, inum, config };
  socket.emit('camera_config', message);
  // console.log(`configCamera: ${JSON.stringify(message)}`);
};
