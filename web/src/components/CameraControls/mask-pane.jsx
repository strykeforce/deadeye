import React from "react";
import { configPipeline } from "../../common/api";
import Range from "./range";

// import "./index.less";


const MaskPane = (props) => {
  const { unit, inum, config } = props;

  const onChange = (name) => (value) => {
    if (value === undefined) return;
    const newConfig = Object.assign(config, { [name]: value });
    configPipeline(unit, inum, newConfig);
  };

  return (
    <>
      <Range name="Hue" values={config.hue} onChange={onChange("hue")} />
      <Range name="Saturation" values={config.sat} onChange={onChange("sat")} />
      <Range name="Value" values={config.val} onChange={onChange("val")} />
    </>
  );
};

export default MaskPane;
