import { Radio } from "antd";
import React from "react";
import { configStream } from "../../common/api";
import { key } from "../../common/util";
import "./camera-stream.less";

const prefix = "camera-stream-controls";

const Controls = (props) => {
  const { unit, inum, config } = props;

  const handleChange = (name) => (event) => {
    const newConfig = Object.assign(config, { [name]: event.target.value });
    configStream(unit, inum, newConfig);
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
            key={key(unit, inum, 9)}
          >
            None
          </Radio>
          <Radio
            className={`${prefix}__radio`}
            value="original"
            onChange={handleChange("view")}
            key={key(unit, inum, 10)}
          >
            Original
          </Radio>
          <Radio
            className={`${prefix}__radio`}
            value="mask"
            onChange={handleChange("view")}
            key={key(unit, inum, 11)}
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
            key={key(unit, inum, 12)}
          >
            None
          </Radio>
          <Radio
            className={`${prefix}__radio`}
            value="filter"
            onChange={handleChange("contour")}
            key={key(unit, inum, 13)}
          >
            Filtered
          </Radio>
          <Radio
            className={`${prefix}__radio`}
            value="all"
            onChange={handleChange("contour")}
            key={key(unit, inum, 14)}
          >
            All
          </Radio>
        </Radio.Group>
      </div>
    </div>
  );
};

export default Controls;
