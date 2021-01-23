import { Col, Input, Row } from "antd";
import React, { useState } from "react";
import { configCapture, enableCamera } from "../../../common/api";
import { key } from "../../../common/util";
import "../camera-controls.less";
import { CaptureControlProps } from "../capture-pane";

const UsbConfigSubPane = (props: CaptureControlProps) => {
  return (
    <>
      <Row>
        <Col span={24}>
          <DeviceInput {...props} />
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
    <>
      <Row className="capture-pane-control">
        <Col span={24} className="capture-pane-control__label">
          Device&nbsp;Path
        </Col>
      </Row>
      <Row className="capture-pane-control">
        <Col span={24}>
          <Input
            value={value}
            onChange={handleChange}
            onPressEnter={saveChange}
            onBlur={saveChange}
            key={key(unit, inum, 18)}
          />
        </Col>
      </Row>
    </>
  );
};
