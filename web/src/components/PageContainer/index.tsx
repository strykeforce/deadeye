import { Layout, Typography } from "antd";
import React, { useState } from "react";
import { Camera, Id, Units } from "../../util/models";
import CameraDashboard from "../CameraDashboard";
import CameraHeader from "../CameraHeader";
import CameraMenu from "../CameraMenu";
import "./index.less";

const { Header, Content, Footer, Sider } = Layout;
const { Text, Link } = Typography;

type Props = {
  units: Units;
};

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export const get = (p: string[]) => (o: any) =>
  p.reduce((xs, x) => (xs && xs[x] ? xs[x] : null), o);

const PageContainer = (props: Props) => {
  const { units } = props;
  const [selectedId, setSelectedId] = useState<Id>(undefined);

  const getCamera = (id: string): Camera => {
    const u = id.charAt(0);
    const c = id.charAt(1);
    return get([u, "cameras", c])(units);
  };

  const ids = Object.values(units)
    .flatMap((u) => Object.values(u.cameras).map((c) => c.id))
    .sort();

  if (!selectedId && ids.length > 0) {
    setSelectedId(ids[0]);
  }

  const content = selectedId ? (
    <CameraDashboard camera={getCamera(selectedId)} />
  ) : null;

  const header = selectedId ? (
    <CameraHeader camera={getCamera(selectedId)} />
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
