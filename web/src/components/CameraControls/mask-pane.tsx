import React from "react";
import { PipelineConfig } from "../../common/models";
import Range from "./range";
import { configPipeline } from "../../common/api";

// import "./index.less";

type Props = {
  unit: string;
  inum: number;
  config: PipelineConfig;
};

const MaskPane = (props: Props) => {
  const { unit, inum, config } = props;

  const onChange = (name: string) => (value: [number, number] | undefined) => {
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
