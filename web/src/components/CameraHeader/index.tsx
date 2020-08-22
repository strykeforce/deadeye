import { Switch } from "antd";
import { SwitchClickEventHandler } from "antd/lib/switch";
import React from "react";
import { enableCamera, enableLight } from "../../common/api";
import { Camera } from "../../common/models";
import "./camera-header.less";
import { RouteComponentProps } from "@reach/router";

interface Props extends RouteComponentProps {
  camera?: Camera;
}

const CameraHeader = (props: Props) => {
  const { camera } = props;

  const handleCameraChange: SwitchClickEventHandler = (checked) => {
    if (camera) enableCamera(camera.unit, camera.inum, checked);
  };

  const handleLightChange: SwitchClickEventHandler = (checked) => {
    if (camera) enableLight(camera.unit, camera.inum, checked);
  };

  const name = camera ? camera.info.pipeline : "::Unknown";

  return (
    <div className="camera-header">
      {camera && (
        <span className="camera-header__title">
          {camera.id} <span>{name.substring(name.lastIndexOf("::") + 2)}</span>
        </span>
      )}
      <div>
        <Switch
          className="camera-header__switch"
          checked={camera && camera.on}
          onClick={handleCameraChange}
        />
        <span className="camera-header__label">Enabled</span>
        <Switch
          className="camera-header__switch"
          checked={camera && camera.light.on}
          onClick={handleLightChange}
        />
        <span className="camera-header__label">Lights</span>
      </div>
    </div>
  );
};

export default CameraHeader;
