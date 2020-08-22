import { RouteComponentProps, Redirect, useNavigate } from "@reach/router";
import { Units } from "../../common/models";
import PageContainer from "../../components/PageContainer";
import React, { useState } from "react";
import { Layout } from "antd";
import CameraDashboard from "../../components/CameraDashboard";
import { getCamera } from "../../common/util";
import CameraHeader from "../../components/CameraHeader";

const { Header, Content } = Layout;

interface Props extends RouteComponentProps {
  id?: string;
  units: Units;
}

const CameraPage = (props: Props) => {
  const { units, id } = props;
  const navigate = useNavigate();

  const ids = Object.values(units)
    .flatMap((u) => Object.values(u.cameras).map((c) => c.id))
    .sort();

  const selectedId = id ? id : ids[0];

  if (!ids.includes(selectedId)) {
    const route = `/id/${ids[0]}`;
    console.debug(`route = ${route}`);
    navigate(route);
    return null;
  }

  const camera = getCamera(selectedId, units);

  return (
    <PageContainer ids={ids}>
      <Header className="page__header">
        <CameraHeader camera={camera} />
      </Header>
      <Content className="page__content">
        <CameraDashboard camera={camera} />
      </Content>
    </PageContainer>
  );
};

export default CameraPage;
