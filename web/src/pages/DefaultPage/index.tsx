import { RouteComponentProps } from "@reach/router";
import { Layout } from "antd";
import React from "react";
import CameraHeader from "../../components/CameraHeader";
import PageContainer from "../../components/PageContainer";
import { Units } from "../../common/models";

const { Header, Content } = Layout;

interface Props extends RouteComponentProps {
  units?: Units;
}

const DefaultPage = (props: Props) => {
  const { units } = props;

  const ids = units
    ? Object.values(units)
        .flatMap((u) => Object.values(u.cameras).map((c) => c.id))
        .sort()
    : undefined;

  return (
    <PageContainer ids={ids}>
      <Header className="page__header">
        <CameraHeader />
      </Header>
      <Content className="page__content">Loading...</Content>
    </PageContainer>
  );
};

export default DefaultPage;
