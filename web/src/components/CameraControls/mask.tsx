import { Slider, InputNumber, Row, Col, Table } from "antd";
import React from "react";
import { PipelineConfig } from "../../common/models";

// import "./index.less";

type Props = {
  config: PipelineConfig;
};

const MaskPane = (props: Props) => {
  const { config } = props;

  const inputValue: [number, number] = [10, 200];

  return (
    <Row>
      <Col span={3}>Saturation</Col>
      <Col span={4}>
        <InputNumber
          // size="small"
          min={1}
          max={255}
          style={{ margin: "0 16px", width: "75px" }}
          value={0}
          // onChange={this.onChange}
        />
      </Col>

      <Col span={13}>
        <Slider
          min={0}
          max={255}
          range
          // onChange={this.onChange}
          value={inputValue}
        />
      </Col>

      <Col span={4}>
        <InputNumber
          // size="small"
          min={1}
          max={255}
          style={{ margin: "0 16px", width: "75px" }}
          value={255}
          // onChange={this.onChange}
        />
      </Col>
    </Row>
  );
};

/*
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
*/

export default MaskPane;
