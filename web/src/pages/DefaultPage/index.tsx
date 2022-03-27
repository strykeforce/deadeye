import { navigate, RouteComponentProps } from "@reach/router";
import { Spin } from "antd";
import React from "react";
import { Units } from "../../common/models";
import { getIds } from "../../common/util";

interface Props extends RouteComponentProps {
  id: string;
  units?: Units;
}

const DefaultPage = (props: Props): JSX.Element | null => {
  const { id, units } = props;

  const ids = getIds(units);

  if (ids.length > 0) {
    const route = ids.includes(id) ? `/id/${id}` : `/id/${ids[0]}`;
    console.info(
      `default page: route = ${route}, id=${id}, ids=${JSON.stringify(ids)}`
    );
    // noinspection JSIgnoredPromiseFromCall
    void navigate(route);
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
      data-testid="default-page"
    >
      <Spin size="large" />
    </div>
  );
};

export default DefaultPage;
