import { Tabs } from "antd";
import React from "react";
import LinkPane from "./link-pane";
import "./settings-dashboard.less";

const { TabPane } = Tabs;

const SettingsDashboard = () => {
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
