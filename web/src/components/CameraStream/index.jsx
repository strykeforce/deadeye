import React from "react";
import "./camera-stream.less";
import Controls from "./controls";
import Image from "./image";


const CameraStream = (props) => {
  const { unit, inum, enabled, config } = props;

  const streamEnabled =
    enabled && (config.view !== "none" || config.contour !== "none");

  return (
    <div className="camera-stream">
      <Image enabled={streamEnabled} url={config.url} />
      <Controls unit={unit} inum={inum} config={config} />
    </div>
  );
};

export default CameraStream;
