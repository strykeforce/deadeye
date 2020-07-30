import { Tabs } from "antd";
import React from "react";
import { Camera } from "../../common/models";
import CapturePane from "./capture";
import FilterPane from "./filter";
import "./camera-controls.less";
import MaskPane from "./mask";

type Props = {
  camera: Camera;
};

const CameraControls = (props: Props) => {
  const { camera } = props;
  const { TabPane } = Tabs;

  return (
    <Tabs className="camera-controls" defaultActiveKey="1">
      <TabPane tab="Capture" key="1">
        <CapturePane
          unit={camera.unit}
          inum={camera.inum}
          config={camera.capture}
        />
      </TabPane>
      <TabPane tab="Mask" key="2">
        <MaskPane
          unit={camera.unit}
          inum={camera.inum}
          config={camera.pipeline}
        />
      </TabPane>
      <TabPane tab="Filter" key="3">
        <FilterPane
          unit={camera.unit}
          inum={camera.inum}
          config={camera.pipeline}
        />
      </TabPane>
    </Tabs>
  );
};

export default CameraControls;
