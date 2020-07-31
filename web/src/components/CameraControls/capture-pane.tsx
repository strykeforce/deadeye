import { message, Col, Row, Select, Slider } from "antd";
import React, { useState } from "react";
import { configCapture, enableCamera } from "../../common/api";
import { CaptureConfig } from "../../common/models";
import "./camera-controls.less";
import TestConfigSubPane from "./capture/test";
import DebugPane from "./debug";

type Props = {
  unit: string;
  inum: number;
  config: CaptureConfig;
  debug?: boolean;
};

const CapturePane = (props: Props) => {
  const { debug = false } = props;

  const [hasRestartDisplayed, setRestartDisplayed] = useState(false);

  const displayRestartMessage = () => {
    if (!hasRestartDisplayed) {
      message.warn({
        content: "Restart camera capture for changes to take effect.",
        style: {
          marginTop: "10vh",
        },
      });
      setRestartDisplayed(true);
    }
  };

  return (
    <>
      <Row>
        <Col span={11}>
          <Row>
            <Col span={24}>
              <TypeSelect {...props} onChange={displayRestartMessage} />
            </Col>
          </Row>
          <Row>
            <Col span={24}>
              <DimensionSelect {...props} onChange={displayRestartMessage} />
            </Col>
          </Row>
          <Row>
            <Col span={24}>
              <FpsSlider {...props} onChange={displayRestartMessage} />
            </Col>
          </Row>
        </Col>
        <Col span={2}></Col>
        <Col span={11}>
          <TestConfigSubPane {...props} onChange={displayRestartMessage} />
        </Col>
      </Row>
      {debug && (
        <Row style={{ paddingTop: "25px" }}>
          <Col span={24}>
            <DebugPane {...props} section="capture" />
          </Col>
        </Row>
      )}
    </>
  );
};

export default CapturePane;

export type CaptureControlProps = Props & { onChange: () => void };

const TypeSelect = (props: CaptureControlProps) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;
  const { Option } = Select;

  const handleChange = (value: string) => {
    const newConfig = Object.assign(config, { type: value });
    configCapture(unit, inum, newConfig);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  return (
    <>
      <Row className="capture-type">
        <Col span={6} className="capture-type__label">
          Type
        </Col>
        <Col span={18}>
          <Select
            dropdownMatchSelectWidth={false}
            value={config.type}
            onChange={handleChange}
          >
            <Option value="test">Test</Option>
            <Option value="jetson">Jetson</Option>
          </Select>
        </Col>
      </Row>
    </>
  );
};

const DimensionSelect = (props: CaptureControlProps) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;
  const values = [config.w, config.h];
  const { Option } = Select;

  const type = `${values[0]}x${values[1]}`;

  const handleChange = (value: string) => {
    const [w, h] = value.split("x", 2).map((n) => parseInt(n));
    const newConfig = Object.assign(config, { w: w, h: h });
    configCapture(unit, inum, newConfig);
    enableCamera(unit, inum, false);
    displayRestartMessage();
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

const FpsSlider = (props: CaptureControlProps) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const handleChange = (value: number) => {
    const newConfig = Object.assign(config, { fps: value });
    configCapture(unit, inum, newConfig);
    enableCamera(unit, inum, false);
    displayRestartMessage();
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
          tooltipVisible={false}
          onChange={handleChange}
        />
      </Col>
    </Row>
  );
};
