import React from "react";
import { PipelineConfig } from "../../common/models";
import Range from "./range";
import { configPipeline } from "../../common/api";
import { Col, message, Row, Select, Slider, Checkbox, InputNumber } from "antd";
import { key } from "../../common/util";

// import "./index.less";

type Props = {
  unit: string;
  inum: number;
  config: PipelineConfig;
};

const LogPane = (props: Props) => {
  const { unit, inum, config } = props;
  const { log } = config;

  const { Option } = Select;

  const handleChange = (value: number) => {
    const newLog = Object.assign(log, { fps: value });
    const newConfig = Object.assign(config, newLog);
    configPipeline(unit, inum, newConfig);
  };

  return (
    <>
      <Row className="capture-pane-control">
        <Col span={6} className="capture-pane-control__label">
          Log / Second
        </Col>
        <Col span={18}>
          <Select
            value={log.fps}
            onChange={handleChange}
            key={key(unit, inum, 21)}
          >
            <Option value={0}>0</Option>
            <Option value={1}>1</Option>
            <Option value={2}>2</Option>
            <Option value={3}>3</Option>
            <Option value={4}>4</Option>
            <Option value={5}>5</Option>
          </Select>
        </Col>
      </Row>

      <Row className="capture-pane-control">
        <Col span={6} className="capture-pane-control__label">
          Log Directory
        </Col>
        <Col span={18}>{config.log.path}</Col>
      </Row>

      <Row className="capture-pane-control">
        <Col span={6} className="capture-pane-control__label">
          USB Drive Required
        </Col>
        <Col span={18}>
          <Checkbox checked={config.log.mount} />
        </Col>
      </Row>
    </>
  );
};

export default LogPane;
