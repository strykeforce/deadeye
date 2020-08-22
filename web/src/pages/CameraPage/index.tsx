import { RouteComponentProps, useNavigate } from "@reach/router";
import { Layout } from "antd";
import React from "react";
import { Units } from "../../common/models";
import { getCamera, getIds } from "../../common/util";
import CameraDashboard from "../../components/CameraDashboard";
import CameraHeader from "../../components/CameraHeader";
import PageContainer from "../../components/PageContainer";

const { Header, Content } = Layout;

interface Props extends RouteComponentProps {
  id?: string;
  units: Units;
}

const CameraPage = (props: Props) => {
  const { units, id } = props;
  const navigate = useNavigate();

  const ids = getIds(units);

  const selectedId = id ? id : ids[0];

  if (!ids.includes(selectedId)) {
    const route = `/id/${ids[0]}`;
    console.debug(`route = ${route}`);
    navigate(route);
    return null;
  }

  const camera = getCamera(selectedId, units);

  return (
    <PageContainer id={selectedId} ids={ids}>
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
