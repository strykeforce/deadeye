import { Switch } from "antd";
import { SwitchClickEventHandler } from "antd/lib/switch";
import React from "react";
import { enableCamera, enableLight } from "../../common/api";
import { Camera } from "../../common/models";
import "./camera-header.less";

type Props = {
  camera: Camera;
};

const CameraHeader = (props: Props) => {
  const { camera } = props;

  const handleCameraChange: SwitchClickEventHandler = (checked) => {
    enableCamera(camera.unit, camera.inum, checked);
  };

  const handleLightChange: SwitchClickEventHandler = (checked) => {
    enableLight(camera.unit, camera.inum, checked);
  };

  const name = camera.info.pipeline.name;

  return (
    <div className="camera-header">
      <span className="camera-header__title">
        {camera.id} <span>{name.substring(name.lastIndexOf("::") + 2)}</span>
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
