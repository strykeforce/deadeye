import React from "react";
import standBy from "./deadeye.png";
import "./camera-stream.less";

interface Props {
  enabled: boolean;
  url: string;
}

const Image = (props: Props) => {
  const { enabled, url } = props;

  return (
    <img
      src={enabled ? url : standBy}
      height={242}
      width={322}
      style={{ border: "1px solid black" }}
      alt="Stream"
      className="camera-stream-image"
    />
  );
};

const areEqual = (prev: Props, next: Props) => {
  const equal = prev.enabled === next.enabled && prev.url === next.url;
  return equal;
};

export default React.memo(Image, areEqual);
