import React from "react";
import { StreamConfig } from "../../common/models";
import "./camera-stream.less";
import Controls from "./controls";
import Image from "./image";

type Props = {
  unit: string;
  inum: number;
  enabled: boolean;
  config: StreamConfig;
};

const CameraStream = (props: Props): JSX.Element => {
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
