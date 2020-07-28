import { Tabs } from "antd";
import React from "react";
import { Camera } from "../../util/models";
import CapturePane from "./CapturePane";
import FilterPane from "./FilterPane";
import "./index.less";
import MaskPane from "./MaskPane";

type Props = {
  camera: Camera;
};

const CameraControls = (props: Props) => {
  const { camera } = props;
  const { TabPane } = Tabs;

  return (
    <Tabs className="camera-controls" defaultActiveKey="1">
      <TabPane tab="Capture" key="1">
        <CapturePane config={camera.capture} />
      </TabPane>
      <TabPane tab="Mask" key="2">
        <MaskPane config={camera.pipeline} />
      </TabPane>
      <TabPane tab="Filter" key="3">
        <FilterPane config={camera.pipeline.filter} />
      </TabPane>
    </Tabs>
  );
};

export default CameraControls;
