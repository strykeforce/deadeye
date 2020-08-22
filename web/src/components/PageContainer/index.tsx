import { Layout, Typography } from "antd";
import React, { useState } from "react";
import { Id, Units } from "../../common/models";
import { getCamera } from "../../common/util";
import CameraDashboard from "../CameraDashboard";
import CameraHeader from "../CameraHeader";
import CameraMenu from "../CameraMenu";
import "./page-container.less";
import { Router, RouteComponentProps } from "@reach/router";

const { Footer, Sider } = Layout;
const { Text, Link } = Typography;

interface Props extends RouteComponentProps {
  ids?: string[];
  children: React.ReactNode;
}

const PageContainer = (props: Props) => {
  const { ids, children } = props;

  return (
    <Layout className="page">
      <Sider className="page__sider" width={100}>
        <CameraMenu ids={ids} />
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
    {" © 2020"}
  </Text>
);

export default PageContainer;
