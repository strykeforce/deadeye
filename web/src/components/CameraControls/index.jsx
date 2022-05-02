import { Tabs } from "antd";
import React from "react";
import { key } from "../../common/util";
import "./camera-controls.less";
import CapturePane from "./capture-pane";
import FilterPane from "./filter-pane";
import LogPane from "./log-pane";
import MaskPane from "./mask-pane";



const { TabPane } = Tabs;

const CameraControls = (props) => {
  const { camera } = props;

  return (
    <Tabs
      className="camera-controls"
      defaultActiveKey={key(camera.unit, camera.inum, 1)}
    >
      <TabPane tab="Capture" key={key(camera.unit, camera.inum, 1)}>
        <CapturePane
          unit={camera.unit}
          inum={camera.inum}
          config={camera.capture}
        />
      </TabPane>
      <TabPane tab="Mask" key={key(camera.unit, camera.inum, 2)}>
        <MaskPane
          unit={camera.unit}
          inum={camera.inum}
          config={camera.pipeline}
        />
      </TabPane>
      <TabPane tab="Filter" key={key(camera.unit, camera.inum, 3)}>
        <FilterPane
          unit={camera.unit}
          inum={camera.inum}
          config={camera.pipeline}
        />
      </TabPane>
      <TabPane tab="Log" key={key(camera.unit, camera.inum, 20)}>
        <LogPane
          unit={camera.unit}
          inum={camera.inum}
          config={camera.pipeline}
        />
      </TabPane>
    </Tabs>
  );
};

export default CameraControls;
