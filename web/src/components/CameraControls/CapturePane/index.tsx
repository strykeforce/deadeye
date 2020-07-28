import { Table } from "antd";
import React from "react";
import { CaptureConfig } from "../../../util/models";
// import "./index.less";

type Props = {
  config: CaptureConfig;
};

const CapturePane = (props: Props) => {
  const { config } = props;
  const data = [
    { key: "1", name: "Type", value: config.type },
    { key: "2", name: "Size", value: `${config.w}x${config.h}` },
    { key: "3", name: "FPS", value: config.fps },
  ];

  const columns = [
    {
      title: "Name",
      dataIndex: "name",
      key: "name",
    },
    {
      title: "Value",
      dataIndex: "value",
      key: "value",
    },
  ];

  return <Table dataSource={data} columns={columns} pagination={false} />;
};

export default CapturePane;
