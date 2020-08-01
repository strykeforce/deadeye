import { Layout, Typography } from "antd";
import React, { useState } from "react";
import { Id, Units } from "../../common/models";
import { getCamera } from "../../common/util";
import CameraDashboard from "../CameraDashboard";
import CameraHeader from "../CameraHeader";
import CameraMenu from "../CameraMenu";
import "./page-container.less";

const { Header, Content, Footer, Sider } = Layout;
const { Text, Link } = Typography;

type Props = {
  units: Units;
};

const PageContainer = (props: Props) => {
  const { units } = props;
  const [selectedId, setSelectedId] = useState<Id>(undefined);

  const ids = Object.values(units)
    .flatMap((u) => Object.values(u.cameras).map((c) => c.id))
    .sort();

  if (!selectedId && ids.length > 0) {
    // setSelectedId(ids[0]);
    setSelectedId("E0"); // FIXME
  }

  const content = selectedId ? (
    <CameraDashboard camera={getCamera(selectedId, units)} />
  ) : null;

  const header = selectedId ? (
    <CameraHeader camera={getCamera(selectedId, units)} />
  ) : null;

  return (
    <Layout className="page">
      <Sider className="page__sider" width={100}>
        <CameraMenu
          ids={ids}
          selectedId={selectedId}
          onClick={(id) => setSelectedId(id)}
        />
      </Sider>
      <Layout>
        <Header className="page__header">{header}</Header>
        <Content className="page__content">{content}</Content>
        <Footer className="page__footer">
          <MadeWithLove />
        </Footer>
      </Layout>
    </Layout>
  );
};

const MadeWithLove = () => (
  <Text type="secondary">
    <Link type="secondary" target="_blank" href="https://www.strykeforce.org/">
      Stryke Force
    </Link>
    {" © 2020"}
  </Text>
);

export default PageContainer;
