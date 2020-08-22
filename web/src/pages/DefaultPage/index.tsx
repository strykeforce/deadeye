import { RouteComponentProps, navigate } from "@reach/router";
import { Layout, Spin } from "antd";
import React from "react";
import CameraHeader from "../../components/CameraHeader";
import PageContainer from "../../components/PageContainer";
import { Units } from "../../common/models";
import { getIds } from "../../common/util";

const { Header, Content } = Layout;

interface Props extends RouteComponentProps {
  id?: string;
  units?: Units;
}

const DefaultPage = (props: Props) => {
  const { id, units } = props;

  const ids = getIds(units);

  if (ids.length > 0 && !ids.includes(id ? id : "")) {
    const route = `/id/${ids[0]}`;
    console.debug(`route = ${route}`);
    navigate(route);
    return null;
  }

  return (
    <div
      style={{
        position: "absolute",
        top: "50%",
        left: "50%",
        transform: "translateX(-50%) translateY(-50%)",
      }}
    >
      <Spin size="large" />
    </div>
  );
};

export default DefaultPage;
