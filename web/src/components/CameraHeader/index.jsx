import { Switch } from "antd";
import React from "react";
import { enableCamera, enableLight } from "../../common/api";
import "./camera-header.less";

const CameraHeader = (props) => {
  if (!props.camera)
    return (
      <div className="camera-header">
        <span className="camera-header__title">{props.title}</span>
      </div>
    );

  const { camera } = props;

  const handleCameraChange = (checked) => {
    enableCamera(camera.unit, camera.inum, checked);
  };

  const handleLightChange = (checked) => {
    enableLight(camera.unit, camera.inum, checked);
  };

  const name = camera.info.pipeline.substring(
    camera.info.pipeline.lastIndexOf("::") + 2
  );

  return (
    <div className="camera-header">
      <span className="camera-header__title">
        {camera.id} <span>{name}</span>
      </span>

      <div>
        <Switch
          className="camera-header__switch"
          checked={camera.on}
          onClick={handleCameraChange}
        />
        <span className="camera-header__label">Enabled</span>
        <Switch
          className="camera-header__switch"
          checked={camera.light.on}
          onClick={handleLightChange}
        />
        <span className="camera-header__label">Lights</span>
      </div>
    </div>
  );
};

export default CameraHeader;
