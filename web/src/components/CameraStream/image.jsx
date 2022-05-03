import React from "react";
import "./camera-stream.less";
import standBy from "./deadeye.png";

const Image = (props) => {
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

const areEqual = (prev, next) => {
  return prev.enabled === next.enabled && prev.url === next.url;
};

export default React.memo(Image, areEqual);
