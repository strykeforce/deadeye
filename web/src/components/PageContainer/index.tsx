import { RouteComponentProps } from "@reach/router";
import { Layout, Typography } from "antd";
import React from "react";
import CameraMenu from "../CameraMenu";
import "./page-container.less";

const { Footer, Sider } = Layout;
const { Text, Link } = Typography;

interface Props extends RouteComponentProps {
  id?: string;
  ids: string[];
  children: React.ReactNode;
}

const PageContainer = (props: Props) => {
  const { id, ids, children } = props;

  return (
    <Layout className="page">
      <Sider className="page__sider" width={100}>
        <CameraMenu id={id} ids={ids} />
      </Sider>
      <Layout>
        {children}
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
    {" © 2020-2021"}
  </Text>
);

export default PageContainer;
