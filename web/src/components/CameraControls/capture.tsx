import { Select, Slider, Row, Col, InputNumber } from "antd";
import React from "react";
import { CaptureConfig } from "../../common/models";
import { configCapture, enableCamera } from "../../common/api";
import "./camera-controls.less";

type Props = { unit: string; inum: number; config: CaptureConfig };

const CapturePane = (props: Props) => {
  return (
    <>
      <Row>
        <Col span={12}>
          <TypeSelect {...props} />
        </Col>
        <Col span={12}></Col>
      </Row>
      <Row>
        <Col span={12}>
          <DimensionSelect {...props} />
        </Col>
        <Col span={12}></Col>
      </Row>
      <Row>
        <Col span={12}>
          <FpsSlider {...props} />
        </Col>
        <Col span={12}></Col>
      </Row>
    </>
  );
};

export default CapturePane;

const TypeSelect = (props: Props) => {
  const { unit, inum, config } = props;
  const { Option } = Select;

  const handleChange = (value: string) => {
    const newConfig = Object.assign(config, { type: value });
    configCapture(unit, inum, newConfig);
    enableCamera(unit, inum, false);
  };

  return (
    <Row className="capture-type">
      <Col span={6} className="capture-type__label">
        Type
      </Col>
      <Col span={18}>
        <Select value={config.type} onChange={handleChange}>
          <Option value="test">Test</Option>
          <Option value="jetson">Jetson</Option>
        </Select>
      </Col>
    </Row>
  );
};

const DimensionSelect = (props: Props) => {
  const { unit, inum, config } = props;
  const values = [config.w, config.h];
  const { Option } = Select;

  const type = `${values[0]}x${values[1]}`;

  const handleChange = (value: string) => {
    const [w, h] = value.split("x", 2).map((n) => parseInt(n));
    const newConfig = Object.assign(config, { w: w, h: h });
    configCapture(unit, inum, newConfig);
    enableCamera(unit, inum, false);
  };

  return (
    <Row className="frame-dimensions">
      <Col span={6} className="frame-dimensions__label">
        Size
      </Col>
      <Col span={18}>
        <Select value={type} onChange={handleChange}>
          <Option value="1280x720">1280x720</Option>
          <Option value="960x540">960x540</Option>
          <Option value="640x360">640x360</Option>
          <Option value="320x180">320x180</Option>
        </Select>
      </Col>
    </Row>
  );
};

const FpsSlider = (props: Props) => {
  const { unit, inum, config } = props;

  const handleChange = (value: number) => {
    const newConfig = Object.assign(config, { fps: value });
    configCapture(unit, inum, newConfig);
    enableCamera(unit, inum, false);
  };

  const marks = {
    30: "30",
    60: "60",
    90: "90",
    120: "120",
  };

  return (
    <Row className="capture-fps">
      <Col span={6} className="capture-fps__label">
        FPS
      </Col>
      <Col span={18}>
        <Slider
          value={config.fps}
          min={30}
          max={120}
          step={null}
          marks={marks}
          onChange={handleChange}
        />
      </Col>
    </Row>
  );
};
