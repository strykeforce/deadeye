import { Switch } from "antd";
import { SwitchClickEventHandler } from "antd/lib/switch";
import React from "react";
import { enableCamera, enableLight } from "../../common/api";
import { Camera } from "../../common/models";
import "./camera-header.less";
import { RouteComponentProps } from "@reach/router";

interface Props extends RouteComponentProps {
  title?: string;
  camera?: Camera;
}

const CameraHeader = (props: Props) => {
  if (!props.camera)
    return (
      <div className="camera-header">
        <span className="camera-header__title">{props.title}</span>
      </div>
    );

  const { camera } = props;

  const handleCameraChange: SwitchClickEventHandler = (checked) => {
    enableCamera(camera.unit, camera.inum, checked);
  };

  const handleLightChange: SwitchClickEventHandler = (checked) => {
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
