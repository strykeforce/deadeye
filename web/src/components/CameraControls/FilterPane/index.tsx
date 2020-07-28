import React from "react";
import { Table } from "antd";
import { FilterConfig } from "../../../util/models";
// import "./index.less";

type Props = {
  config: FilterConfig;
};

const FilterPane = (props: Props) => {
  const { config } = props;

  const data = [
    { key: "1", name: "Area", min: config.area[0], max: config.area[1] },
    {
      key: "2",
      name: "Solidity",
      min: config.solidity[0],
      max: config.solidity[1],
    },
    {
      key: "3",
      name: "Aspect Ratio",
      min: config.aspect[0],
      max: config.aspect[1],
    },
  ];

  const columns = [
    {
      title: "Filter",
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

export default FilterPane;
