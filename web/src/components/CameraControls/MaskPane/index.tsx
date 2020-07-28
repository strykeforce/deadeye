import { Table } from "antd";
import React from "react";
import { PipelineConfig } from "../../../util/models";

// import "./index.less";

type Props = {
  config: PipelineConfig;
};

const MaskPane = (props: Props) => {
  const { config } = props;

  const data = [
    { key: "1", name: "Hue", min: config.hue[0], max: config.hue[1] },
    { key: "2", name: "Saturation", min: config.sat[0], max: config.sat[1] },
    { key: "3", name: "Value", min: config.val[0], max: config.val[1] },
  ];

  const columns = [
    {
      title: "Value",
      dataIndex: "name",
      key: "name",
    },
    {
      title: "Min",
      dataIndex: "min",
      key: "min",
    },
    {
      title: "Max",
      dataIndex: "max",
      key: "max",
    },
  ];

  return <Table dataSource={data} columns={columns} pagination={false} />;
};

export default MaskPane;
