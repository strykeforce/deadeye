import { Col, Input, InputNumber, Row, Space } from "antd";
import Checkbox, { CheckboxChangeEvent } from "antd/lib/checkbox/Checkbox";
import React, { useState } from "react";
import { configCapture, enableCamera } from "../../../common/api";
import { key } from "../../../common/util";
import "../camera-controls.less";
import { CaptureControlProps } from "../capture-pane";

const UsbConfigSubPane = (props: CaptureControlProps): JSX.Element => {
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

const DeviceInput = (props: CaptureControlProps) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const innerConfig = config.config as {
    device?: string;
  };

  const [value, setValue] = useState<string>(
    innerConfig.device || "/dev/video0"
  );

  const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
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

const ExposureControls = (props: CaptureControlProps) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const innerConfig = config.config as {
    aeLock?: boolean;
    exposure?: number;
  };

  const aeLock = innerConfig.aeLock !== undefined ? innerConfig.aeLock : true;
  const exposure = innerConfig.exposure;

  const handleAELockChange = (e: CheckboxChangeEvent) => {
    const newConfig = Object.assign(innerConfig, { aeLock: e.target.checked });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  const handleExposureChange = (value: string | number | null | undefined) => {
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

const BrightnessInput = (props: CaptureControlProps) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const innerConfig = config.config as {
    brightness?: number;
  };

  const brightness = innerConfig.brightness;

  const handleChange = (value: string | number | null | undefined) => {
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

const WhiteBalanceControls = (props: CaptureControlProps) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const innerConfig = config.config as {
    awbLock?: boolean;
    wbTemp?: number;
  };

  const awbLock =
    innerConfig.awbLock !== undefined ? innerConfig.awbLock : true;
  const wbTemp = innerConfig.wbTemp;

  const handleWBLockChange = (e: CheckboxChangeEvent) => {
    const newConfig = Object.assign(innerConfig, { awbLock: e.target.checked });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  const handleWBTempChange = (value: string | number | null | undefined) => {
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
