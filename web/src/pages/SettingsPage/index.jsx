import { Layout } from "antd";
import React from "react";
import { getIds } from "../../common/util";
import CameraHeader from "../../components/CameraHeader";
import PageContainer from "../../components/PageContainer";
import SettingsDashboard from "../../components/SettingsDashboard";

const { Header, Content } = Layout;

const SettingsPage = (props) => {
  const { units } = props;

  return (
    <PageContainer id="settings" ids={getIds(units)}>
      <Header className="page__header">
        <CameraHeader title="Settings" />
      </Header>
      <Content className="page__content">
        <SettingsDashboard />
      </Content>
    </PageContainer>
  );
};

export default SettingsPage;
