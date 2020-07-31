import { Col, Input, Row, Select } from "antd";
import React from "react";
import { configCapture, enableCamera } from "../../../common/api";
import "../camera-controls.less";
import { CaptureControlProps } from "../capture-pane";

const TestConfigSubPane = (props: CaptureControlProps) => {
  return (
    <>
      <Row>
        <Col span={24}>
          <PatternSelect {...props} />
        </Col>
      </Row>
      <Row>
        <Col span={24}>
          <ColorSelect
            key={`${props.unit}${props.inum}fg`}
            {...props}
            name="foregroundColor"
          />
        </Col>
      </Row>
      <Row>
        <Col span={24}>
          <ColorSelect
            key={`${props.unit}${props.inum}bg`}
            {...props}
            name="backgroundColor"
          />
        </Col>
      </Row>
    </>
  );
};

export default TestConfigSubPane;

const PatternSelect = (props: CaptureControlProps) => {
  const { unit, inum, config, onChange: displayRestartMessage } = props;
  const options = config.config as { pattern: string };
  const { Option } = Select;

  const handleChange = (value: string) => {
    const newConfig = Object.assign(config.config, { pattern: value });
    const newCapture = Object.assign(config, newConfig);
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  return (
    <Row className="test-pattern-type">
      <Col span={12} className="test-pattern-type__label">
        Pattern
      </Col>
      <Col span={12}>
        <Select
          dropdownMatchSelectWidth={false}
          value={options.pattern ? options.pattern : "smpte"}
          onChange={handleChange}
        >
          <Option value="smpte">Color Bars</Option>
          <Option value="ball">Ball</Option>
          <Option value="gradient">Gradient</Option>
        </Select>
      </Col>
    </Row>
  );
};

type ColorSelectProps = CaptureControlProps & { name: string };

class ColorSelect extends React.Component<
  ColorSelectProps,
  { value: string | undefined }
> {
  constructor(props: ColorSelectProps) {
    super(props);
    const innerConfig = this.props.config.config as {
      foregroundColor?: string;
      backgroundColor?: string;
    };
    const init =
      this.props.name === "foregroundColor"
        ? innerConfig.foregroundColor
        : innerConfig.backgroundColor;
    this.state = { value: init };
  }

  handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    this.setState({ value: event.currentTarget.value });
  };

  handlePressEnter = (event: React.KeyboardEvent<HTMLInputElement>) => {};

  saveChanges = () => {
    const {
      unit,
      inum,
      config,
      name,
      onChange: displayRestartMessage,
    } = this.props;
    const newConfig = Object.assign(config.config, {
      [name]: this.state.value,
    });
    const newCapture = Object.assign(config, { config: newConfig });
    configCapture(unit, inum, newCapture);
    enableCamera(unit, inum, false);
    displayRestartMessage();
  };

  render() {
    return (
      <Row className="test-pattern-color">
        <Col span={12} className="test-pattern-color__label">
          {this.props.name === "foregroundColor"
            ? "Foreground Color"
            : "Background Color"}
        </Col>
        <Col span={12}>
          <Input
            // defaultValue={`${unit}${inum}${name}`}
            // defaultValue={
            //   name === "foregroundColor"
            //     ? options.foregroundColor
            //     : options.backgroundColor
            // }
            value={this.state.value}
            // placeholder={name === "foregroundColor" ? "FFFFFF" : "000000"}
            maxLength={6}
            onChange={this.handleChange}
            onPressEnter={() => this.saveChanges()}
            onBlur={() => this.saveChanges()}
          />
        </Col>
      </Row>
    );
  }
}
