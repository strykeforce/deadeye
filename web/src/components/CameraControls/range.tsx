import { Slider, InputNumber, Row, Col } from "antd";
import React from "react";
import "./camera-controls.less";

type Props = {
  name: string;
  min?: number;
  max?: number;
  step?: number;
  values: [number, number];
  onChange: (value: [number, number] | undefined) => void;
};

const Range = (props: Props) => {
  const { name, min = 0, max = 255, step = 1, values, onChange } = props;

  const onMinChange = (value: string | number | null | undefined) => {
    const min = typeof value === "number" ? value : values[0];
    onChange([min, values[1]]);
  };

  const onMaxChange = (value: string | number | null | undefined) => {
    const max = typeof value === "number" ? value : values[1];
    onChange([values[0], max]);
  };
  return (
    <Row className="hsv-range">
      <Col span={3} className="hsv-range__label">
        <span>{name}</span>
      </Col>
      <Col span={4}>
        <InputNumber
          min={min}
          max={max}
          className="hsv-range__input"
          value={values[0]}
          onChange={onMinChange}
        />
      </Col>

      <Col span={13}>
        <Slider
          min={min}
          max={max}
          step={step}
          range
          onChange={onChange}
          value={values}
        />
      </Col>

      <Col span={4}>
        <InputNumber
          min={min}
          max={max}
          className="hsv-range__input"
          value={values[1]}
          onChange={onMaxChange}
        />
      </Col>
    </Row>
  );
};

export default Range;
