import { Col, Input, InputNumber, Row, Space } from "antd";
import Checkbox from "antd/lib/checkbox/Checkbox";
import React, { useState } from "react";
import { configCapture, enableCamera } from "../../../common/api";
import { key } from "../../../common/util";
import "../camera-controls.less";

const UsbConfigSubPane = (props) => {
  return (
    <>
      <Row>
        <Col span={24}>
          <DeviceInput {...props} />
        </Col>
      </Row>
      <Row>
        <Col span={24}>
          <ExposureControls {...props} />
        </Col>
      </Row>
      <Row>
        <Col span={24}>
          <BrightnessInput {...props} />
        </Col>
      </Row>
      <Row>
        <Col span={24}>
          <WhiteBalanceControls {...props} />
        </Col>
      </Row>
    </>
  );
};

export default UsbConfigSubPane;

const DeviceInput = (props) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const innerConfig = config.config;

  const [value, setValue] = useState < string > (
    innerConfig.device || "/dev/video0"
  );

  const handleChange = (event) => {
    setValue(event.currentTarget.value);
  };

  const saveChange = () => {
    const newConfig = Object.assign(innerConfig, { device: value });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  return (
    <Row className="capture-pane-control">
      <Col span={7} className="capture-pane-control__label">
        Device
      </Col>
      <Col span={17}>
        <Input
          value={value}
          onChange={handleChange}
          onPressEnter={saveChange}
          onBlur={saveChange}
          key={key(unit, inum, 18)}
        />
      </Col>
    </Row>
  );
};

const ExposureControls = (props) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const innerConfig = config.config;

  const aeLock = innerConfig.aeLock !== undefined ? innerConfig.aeLock : true;
  const exposure = innerConfig.exposure;

  const handleAELockChange = (e) => {
    const newConfig = Object.assign(innerConfig, { aeLock: e.target.checked });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  const handleExposureChange = (value) => {
    const n = Number(value);
    const newConfig = Object.assign(innerConfig, { exposure: n });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  return (
    <Row className="capture-pane-control">
      <Col span={7} className="capture-pane-control__label">
        Exposure
      </Col>
      <Col span={17}>
        <Space size="large">
          <InputNumber
            min={0}
            max={100}
            value={exposure}
            disabled={aeLock}
            onChange={handleExposureChange}
          />
          <Checkbox checked={aeLock} onChange={handleAELockChange}>
            Auto
          </Checkbox>
        </Space>
      </Col>
    </Row>
  );
};

const BrightnessInput = (props) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const innerConfig = config.config;

  const brightness = innerConfig.brightness;

  const handleChange = (value) => {
    const n = Number(value);
    const newConfig = Object.assign(innerConfig, { brightness: n });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  return (
    <Row className="capture-pane-control">
      <Col span={7} className="capture-pane-control__label">
        Brightness
      </Col>
      <Col span={17}>
        <InputNumber
          min={0}
          max={100}
          value={brightness}
          onChange={handleChange}
        />
      </Col>
    </Row>
  );
};

const WhiteBalanceControls = (props) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const innerConfig = config.config;

  const awbLock =
    innerConfig.awbLock !== undefined ? innerConfig.awbLock : true;
  const wbTemp = innerConfig.wbTemp;

  const handleWBLockChange = (e) => {
    const newConfig = Object.assign(innerConfig, { awbLock: e.target.checked });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  const handleWBTempChange = (value) => {
    const n = Number(value);
    const newConfig = Object.assign(innerConfig, { wbTemp: n });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };
  return (
    <Row className="capture-pane-control">
      <Col span={7} className="capture-pane-control__label">
        White Bal.
      </Col>
      <Col span={17}>
        <Space size="large">
          <InputNumber
            min={2000}
            max={10000}
            value={wbTemp}
            disabled={awbLock}
            onChange={handleWBTempChange}
          />
          <Checkbox checked={awbLock} onChange={handleWBLockChange}>
            Auto
          </Checkbox>
        </Space>
      </Col>
    </Row>
  );
};
