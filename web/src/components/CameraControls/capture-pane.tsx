import { Col, message, Row, Select, Slider } from "antd";
import React, { useState } from "react";
import { configCapture, enableCamera } from "../../common/api";
import { CaptureConfig } from "../../common/models";
import { key } from "../../common/util";
import "./camera-controls.less";
import JetsonConfigSubPane from "./capture/jetson";
import TestConfigSubPane from "./capture/test";
import DebugPane from "./debug";
import UploadConfigSubPane from "./capture/upload";

type Props = {
  unit: string;
  inum: number;
  config: CaptureConfig;
};

const CapturePane = (props: Props) => {
  const {
    config: { type },
  } = props;

  const debug = false;

  const [hasRestartDisplayed, setRestartDisplayed] = useState(false);

  const displayRestartMessage = () => {
    if (!hasRestartDisplayed) {
      message.warn({
        content: "Restart camera capture for changes to take effect.",
        style: {
          marginTop: "8vh",
        },
      });
      setRestartDisplayed(true);
    }
  };

  const subPane = () => {
    switch (type) {
      case "test":
        return (
          <TestConfigSubPane {...props} onChange={displayRestartMessage} />
        );
      case "jetson":
        return (
          <JetsonConfigSubPane {...props} onChange={displayRestartMessage} />
        );
      case "file":
        return (
          <UploadConfigSubPane {...props} onChange={displayRestartMessage} />
        );
      default:
        return null;
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
        <Col span={11}>{subPane()}</Col>
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
      <Row className="capture-pane-control">
        <Col span={6} className="capture-pane-control__label">
          Type
        </Col>
        <Col span={18}>
          <Select
            dropdownMatchSelectWidth={false}
            value={config.type}
            onChange={handleChange}
            key={key(unit, inum, 4)}
          >
            <Option value="test">Test Pattern</Option>
            <Option value="jetson">Jetson Nano</Option>
            <Option value="file">Image Upload</Option>
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
    <Row className="capture-pane-control">
      <Col span={6} className="capture-pane-control__label">
        Output
      </Col>
      <Col span={18}>
        <Select value={type} onChange={handleChange} key={key(unit, inum, 5)}>
          <Option value="1280x720">1280 x 720 px</Option>
          <Option value="960x540">960 x 540 px</Option>
          <Option value="640x360">640 x 360 px</Option>
          <Option value="320x180">320 x 180 px</Option>
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
    <Row className="capture-pane-control">
      <Col span={6} className="capture-pane-control__label">
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
