import { Tabs } from "antd";
import React from "react";
import "./settings-dashboard.less";
import LinkPane from "./link-pane";

type Props = {};
const { TabPane } = Tabs;

const SettingsDashboard = (_: Props) => {
  return (
    <div className="settings-dash">
      <Tabs className="settings-dash__controls" defaultActiveKey="S1">
        <TabPane tab="Target Data" key="S1">
          <LinkPane />
        </TabPane>
      </Tabs>
    </div>
  );
};

export default SettingsDashboard;
