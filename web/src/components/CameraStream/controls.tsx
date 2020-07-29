import { Radio } from "antd";
import { RadioChangeEvent } from "antd/lib/radio";
import React from "react";
import { configStream } from "../../common/api";
import { StreamConfig } from "../../common/models";
import "./camera-stream.less";

type Props = {
  unit: string;
  inum: number;
  config: StreamConfig;
};

const prefix = "camera-stream-controls";

const Controls = (props: Props) => {
  const { unit, inum, config } = props;

  const handleChange = (name: string) => (event: RadioChangeEvent) => {
    const newConfig = Object.assign(config, { [name]: event.target.value });
    configStream(unit, inum, newConfig);
    console.debug(`name = ${name} value = ${event.target.value}`);
  };

  return (
    <div className={prefix}>
      <div className={`${prefix}__group`}>
        <span>View</span>
        <Radio.Group value={config.view}>
          <Radio
            className={`${prefix}__radio`}
            value="none"
            onChange={handleChange("view")}
            key={1}
          >
            None
          </Radio>
          <Radio
            className={`${prefix}__radio`}
            value="original"
            onChange={handleChange("view")}
            key={2}
          >
            Original
          </Radio>
          <Radio
            className={`${prefix}__radio`}
            value="mask"
            onChange={handleChange("view")}
            key={3}
          >
            Mask
          </Radio>
        </Radio.Group>
      </div>

      <div className={`${prefix}__group`}>
        <span>Contours</span>
        <Radio.Group value={config.contour}>
          <Radio
            className={`${prefix}__radio`}
            value="none"
            onChange={handleChange("contour")}
            key={4}
          >
            None
          </Radio>
          <Radio
            className={`${prefix}__radio`}
            value="filter"
            onChange={handleChange("contour")}
            key={5}
          >
            Filtered
          </Radio>
          <Radio
            className={`${prefix}__radio`}
            value="all"
            onChange={handleChange("contour")}
            key={6}
          >
            All
          </Radio>
        </Radio.Group>
      </div>
    </div>
  );
};

export default Controls;
