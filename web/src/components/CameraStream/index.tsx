import React from "react";
import { StreamConfig } from "../../util/models";
import Controls from "./Controls";
import Image from "./Image";
import "./index.less";

type Props = {
  unit: string;
  inum: number;
  enabled: boolean;
  config: StreamConfig;
};

const CameraStream = (props: Props) => {
  const { unit, inum, enabled, config } = props;

  const streamEnabled =
    enabled && (config.view !== "none" || config.contour !== "none");
  console.debug(`streamEnabled = ${streamEnabled}`);

  return (
    <div className="camera-stream">
      <Image enabled={streamEnabled} url={config.url} />
      <Controls unit={unit} inum={inum} config={config} />
    </div>
  );
};

export default CameraStream;
