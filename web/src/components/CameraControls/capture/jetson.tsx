import { Col, InputNumber, Row, Select } from "antd";
import React from "react";
import { configCapture, enableCamera } from "../../../common/api";
import { key } from "../../../common/util";
import "../camera-controls.less";
import { CaptureControlProps } from "../capture-pane";

// capture frame dimensions ✔
// exposure ✔
// gain range ✔
// digital gain range ✔
// saturation ✔
// wb mode?
// flip

const JetsonConfigSubPane = (props: CaptureControlProps) => {
  return (
    <>
      <Row>
        <Col span={24}>
          <DimensionSelect {...props} />
        </Col>
      </Row>
      <Row>
        <Col span={24}>
          <Float {...props} name="exposure" min={0} max={1} />
        </Col>
      </Row>
      <Row>
        <Col span={24}>
          <GainRange {...props} name="gainRange" />
        </Col>
      </Row>
      <Row>
        <Col span={24}>
          <GainRange {...props} name="ispDigitalGainRange" />
        </Col>
      </Row>
      <Row>
        <Col span={24}>
          <Float {...props} name="saturation" min={0} max={2} />
        </Col>
      </Row>
      <Row>
        <Col span={24}>
          <FlipModeSelect {...props} />
        </Col>
      </Row>
    </>
  );
};

export default JetsonConfigSubPane;

const DimensionSelect = (props: CaptureControlProps) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const innerConfig = config.config as {
    captureWidth?: number;
    captureHeight?: number;
  };

  const values = [
    innerConfig.captureWidth ? innerConfig.captureWidth : 1280,
    innerConfig.captureHeight ? innerConfig.captureHeight : 720,
  ];
  const { Option } = Select;

  const type = `${values[0]}x${values[1]}`;

  const handleChange = (value: string) => {
    const [w, h] = value.split("x", 2).map((n) => parseInt(n));
    const newConfig = Object.assign(innerConfig, {
      captureWidth: w,
      captureHeight: h,
    });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  return (
    <Row className="capture-pane-control">
      <Col span={6} className="capture-pane-control__label">
        Capture
      </Col>
      <Col span={18}>
        <Select value={type} onChange={handleChange} key={key(unit, inum, 6)}>
          <Option value="3264x2464">3264 x 2464 px</Option>
          <Option value="3264x1848">3264 x 1848 px</Option>
          <Option value="1920x1080">1920 x 1080 px</Option>
          <Option value="1280x720">1280 x 720 px</Option>
        </Select>
      </Col>
    </Row>
  );
};

type FloatProps = CaptureControlProps & {
  name: string;
  min: number;
  max: number;
};

const Float = (props: FloatProps) => {
  const {
    unit,
    inum,
    config,
    onChange: displayRestartMessage,
    name,
    min,
    max,
  } = props;

  const innerConfig = config.config as {
    exposure?: number;
    saturation?: number;
  };

  let value;
  switch (name) {
    case "exposure":
      value = innerConfig.exposure !== undefined ? innerConfig.exposure : 0.1;
      break;
    case "saturation":
      value =
        innerConfig.saturation !== undefined ? innerConfig.saturation : 1.0;
      break;
    default:
      break;
  }

  const handleChange = (value: string | number | undefined) => {
    let val = Number(value);
    if (val > max) {
      val = max;
    } else if (val < min) {
      val = min;
    }
    const newConfig = Object.assign(innerConfig, { [name]: val });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  return (
    <Row className="capture-pane-control">
      <Col span={6} className="capture-pane-control__label">
        {name === "exposure" ? "Exposure" : "Saturation"}
      </Col>
      <Col span={18}>
        <InputNumber
          min={min}
          max={max}
          step={(max - min) / 100}
          value={value}
          onChange={handleChange}
          key={key(unit, inum, 18)}
        />
      </Col>
    </Row>
  );
};

type GainRangeProps = CaptureControlProps & { name: string };

const GainRange = (props: GainRangeProps) => {
  const { unit, inum, config, onChange: displayRestartMessage, name } = props;
  const range = [1, 16];

  const innerConfig = config.config as {
    gainRange?: string;
    ispDigitalGainRange?: string;
  };

  const value =
    name === "gainRange"
      ? innerConfig.gainRange
      : innerConfig.ispDigitalGainRange;

  const [lower, upper] = value
    ? value.split(" ", 2).map((n) => parseInt(n))
    : range;

  const saveChange = (value: string) => {
    const newConfig = Object.assign(innerConfig, {
      [name]: value,
    });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  const handleChange = (which: "lower" | "upper") => (
    value: string | number | undefined
  ) => {
    let n = Number(value);
    if (which === "lower") {
      n = n <= upper ? n : upper;
      saveChange(`${n} ${upper}`);
      return;
    }
    // which === "upper"
    n = n >= lower ? n : lower;
    saveChange(`${lower} ${n}`);
  };

  return (
    <Row className="capture-pane-control">
      <Col span={6} className="capture-pane-control__label">
        {name === "gainRange" ? "Gain" : "ISP Gain"}
      </Col>
      <Col span={8}>
        <InputNumber
          min={range[0]}
          max={range[1]}
          value={lower}
          onChange={handleChange("lower")}
          key={key(unit, inum, 16)}
        />
      </Col>
      <Col span={2}></Col>
      <Col span={8}>
        <InputNumber
          min={range[0]}
          max={range[1]}
          value={upper}
          onChange={handleChange("upper")}
          key={key(unit, inum, 17)}
        />
      </Col>
    </Row>
  );
};

const FlipModeSelect = (props: CaptureControlProps) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;

  const innerConfig = config.config as {
    flipMethod?: number;
  };
  const { Option } = Select;
  const flipMode = innerConfig.flipMethod
    ? innerConfig.flipMethod.toString()
    : "0";

  const handleChange = (value: string) => {
    const newConfig = Object.assign(innerConfig, { flipMethod: Number(value) });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  return (
    <>
      <Row className="capture-pane-control">
        <Col span={6} className="capture-pane-control__label">
          Flip Mode
        </Col>
        <Col span={18}>
          <Select
            dropdownMatchSelectWidth={false}
            value={flipMode}
            onChange={handleChange}
            key={key(unit, inum, 19)}
          >
            <Option value="0">No Rotation</Option>
            <Option value="1">90 degrees CCW</Option>
            <Option value="2">180 degrees</Option>
            <Option value="3">90 degrees CW</Option>
            <Option value="4">Horizontal Flip</Option>
            <Option value="5">Upper Right Diagonal Flip</Option>
            <Option value="6">Vertical Flip</Option>
            <Option value="7">Upper Left Diagonal Flip</Option>
          </Select>
        </Col>
      </Row>
    </>
  );
};
