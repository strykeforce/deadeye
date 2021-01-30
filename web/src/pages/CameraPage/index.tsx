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
  const units = props.units;
  const id = String(props.id);

  const navigate = useNavigate();

  const ids = getIds(units);

  if (ids.length === 0) {
    console.warn("no camera ids, navigating to /");
    navigate("/");
  }

  const selectedId = ids.includes(id) ? id : ids[0];

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
