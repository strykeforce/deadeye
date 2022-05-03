import { Checkbox, Col, Input, Row, Select, Slider } from "antd";
import React, { useState } from "react";
import { configPipeline } from "../../common/api";
import { key } from "../../common/util";
import DebugPane from "./debug";

// import "./index.less";


const LogPane = (props) => {
  const debug = false;
  return (
    <>
      <TypeSelect {...props} />

      <FpsSelect {...props} />

      <DirectoryInput {...props} />

      <MountCheckbox {...props} />

      {debug && (
        <Row style={{ paddingTop: "25px" }}>
          <Col span={24}>
            <DebugPane {...props} section="pipeline" />
          </Col>
        </Row>
      )}
    </>
  );
};

export default LogPane;

const TypeSelect = (props) => {
  const { unit, inum, config } = props;
  const { log } = config;

  const { Option } = Select;

  const handleChange = (value) => {
    const newLog = Object.assign(log, { type: value });
    const newConfig = Object.assign(config, { log: newLog });
    configPipeline(unit, inum, newConfig);
  };

  return (
    <Row className="capture-pane-control">
      <Col span={6} className="capture-pane-control__label">
        Log Type
      </Col>
      <Col span={18}>
        <Select
          dropdownMatchSelectWidth={false}
          value={log.type}
          onChange={handleChange}
        >
          <Option value="capture">Full Frame</Option>
          <Option value="four_up">Four Up</Option>
        </Select>
      </Col>
    </Row>
  );
};

const FpsSelect = (props) => {
  const { unit, inum, config } = props;
  const { log } = config;

  const handleChange = (value) => {
    const newLog = Object.assign(log, { fps: value });
    const newConfig = Object.assign(config, { log: newLog });
    configPipeline(unit, inum, newConfig);
  };

  const marks = {
    0: "OFF",
    1: "1",
    2: "2",
    3: "3",
    4: "4",
    5: "5",
  };

  return (
    <Row className="capture-pane-control">
      <Col span={6} className="capture-pane-control__label">
        Log Rate (FPS)
      </Col>
      <Col span={18}>
        <Slider
          value={log.fps}
          min={0}
          max={5}
          step={null}
          marks={marks}
          tooltipVisible={false}
          onChange={handleChange}
          key={key(unit, inum, 21)}
        />
      </Col>
    </Row>
  );
};

const DirectoryInput = (props) => {
  const { unit, inum, config } = props;
  const { log } = config;

  const [value, setValue] = useState(log.path);

  const handleChange = (event) => {
    setValue(event.currentTarget.value);
  };

  const saveChange = () => {
    const newLog = Object.assign(log, { path: value });
    const newConfig = Object.assign(config, { log: newLog });
    configPipeline(unit, inum, newConfig);
  };

  return (
    <Row className="capture-pane-control">
      <Col span={6} className="capture-pane-control__label">
        Log Directory
      </Col>
      <Col span={18}>
        <Input
          value={value}
          disabled={log.fps === 0}
          onChange={handleChange}
          onPressEnter={saveChange}
          onBlur={saveChange}
          key={key(unit, inum, 22)}
        />
      </Col>
    </Row>
  );
};

const MountCheckbox = (props) => {
  const { unit, inum, config } = props;
  const { log } = config;

  const handleChange = (event) => {
    const value = event.target.checked;
    const newLog = Object.assign(log, { mount: value });
    const newConfig = Object.assign(config, { log: newLog });
    configPipeline(unit, inum, newConfig);
  };

  return (
    <Row className="capture-pane-control">
      <Col span={6} className="capture-pane-control__label">
        USB Drive Required
      </Col>
      <Col span={18}>
        <Checkbox
          checked={log.mount}
          disabled={log.fps === 0}
          onChange={handleChange}
        />
      </Col>
    </Row>
  );
};
