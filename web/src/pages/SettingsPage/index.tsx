import { Layout } from "antd";
import { RouteComponentProps } from "@reach/router";
import React from "react";
import PageContainer from "../../components/PageContainer";
import { Units } from "../../common/models";
import { getIds } from "../../common/util";
import CameraHeader from "../../components/CameraHeader";
import SettingsDashboard from "../../components/SettingsDashboard";

const { Header, Content } = Layout;

interface Props extends RouteComponentProps {
  units: Units;
}

const SettingsPage = (props: Props) => {
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
