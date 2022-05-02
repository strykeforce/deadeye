import React from "react";
import CameraControls from "../CameraControls";
import CameraStream from "../CameraStream";
import "./camera-dashboard.less";

const CameraDashboard = (props) => {
  const { camera } = props;

  return (
    <div className="camera-dash">
      <div className="camera-dash__controls">
        <CameraControls camera={camera} />
      </div>
      <div className="camera-dash__stream">
        <CameraStream
          unit={camera.unit}
          inum={camera.inum}
          enabled={camera.on}
          config={camera.stream}
        />
      </div>
    </div>
  );
};

export default CameraDashboard;
