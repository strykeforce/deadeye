import { useNavigate } from "@gatsbyjs/reach-router";
import { Layout } from "antd";
import React from "react";
import { getCamera, getIds } from "../../common/util";
import CameraDashboard from "../../components/CameraDashboard";
import CameraHeader from "../../components/CameraHeader";
import PageContainer from "../../components/PageContainer";

const { Header, Content } = Layout;

const CameraPage = (props) => {
  const units = props.units;
  const id = String(props.id);

  const navigate = useNavigate();

  const ids = getIds(units);

  if (ids.length === 0) {
    console.warn("no camera ids, navigating to /");
    // noinspection JSIgnoredPromiseFromCall
    void navigate("/");
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
