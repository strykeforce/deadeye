import { Col, message, Row, Select, Slider } from "antd";
import React, { useState } from "react";
import { configCapture, enableCamera } from "../../common/api";
import { key, messageOffset } from "../../common/util";
import "./camera-controls.less";
import JetsonConfigSubPane from "./capture/jetson";
import TestConfigSubPane from "./capture/test";
import UploadConfigSubPane from "./capture/upload";
import UsbConfigSubPane from "./capture/usb";
import DebugPane from "./debug";


const CapturePane = (props) => {
  const {
    config: { type },
  } = props;

  const debug = false;

  const [hasRestartDisplayed, setRestartDisplayed] = useState(false);

  const displayRestartMessage = () => {
    if (!hasRestartDisplayed) {
      // noinspection JSIgnoredPromiseFromCall
      void message.warn({
        content: "Restart camera capture for changes to take effect.",
        style: {
          marginTop: messageOffset,
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
      case "usb":
        return <UsbConfigSubPane {...props} onChange={displayRestartMessage} />;
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
        <Col span={2} />
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

const TypeSelect = (props) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;
  const { Option } = Select;

  const handleChange = (value) => {
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
            <Option value="usb">USB Camera</Option>
            <Option value="file">Image Upload</Option>
          </Select>
        </Col>
      </Row>
    </>
  );
};

const DIMENSIONS = {
  // Microsoft LifeCam: v4l2-ctl -d /dev/video0 --list-formats-ext
  usb: [
    [1280, 720],
    [960, 544],
    [800, 600],
    [800, 448],
    [640, 480],
    [640, 360],
    [416, 240],
    [352, 288],
    [320, 240],
    [176, 144],
    [160, 120],
  ],
  file: [
    [1280, 720],
    [960, 540],
    [640, 480],
    [640, 360],
    [320, 180],
  ],
  test: [
    [1280, 720],
    [720, 1280],
    [960, 540],
    [640, 360],
    [320, 180],
  ],
  jetson: [
    [1280, 720],
    [720, 1280],
    [960, 540],
    [640, 360],
    [320, 180],
  ],
};

const DimensionSelect = (props) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;
  const values = [config.w, config.h];
  const { Option } = Select;

  const dims = `${values[0]}x${values[1]}`;

  const handleChange = (value) => {
    const [w, h] = value.split("x", 2).map((n) => parseInt(n));
    const newConfig = Object.assign(config, { w: w, h: h });
    configCapture(unit, inum, newConfig);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  const options = DIMENSIONS[config.type].map(
    (dims) => {
      const key = `${dims[0]}x${dims[1]}`;
      return (
        <Option value={key} key={key}>
          {dims[0]} x {dims[1]} px
        </Option>
      );
    }
  );

  return (
    <Row className="capture-pane-control">
      <Col span={6} className="capture-pane-control__label">
        Output
      </Col>
      <Col span={18}>
        <Select value={dims} onChange={handleChange} key={key(unit, inum, 5)}>
          {options}
        </Select>
      </Col>
    </Row>
  );
};

const FpsSlider = (props) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const handleChange = (value) => {
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
