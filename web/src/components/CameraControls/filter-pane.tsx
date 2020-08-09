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

const FilterPane = (props: Props) => {
  const { unit, inum, config } = props;
  const { filter } = config;

  const onChange = (name: string) => (value: [number, number] | undefined) => {
    if (value === undefined) return;
    const newFilter = Object.assign(filter, { [name]: value });
    const newConfig = Object.assign(config, { filter: newFilter });
    configPipeline(unit, inum, newConfig);
  };

  return (
    <>
      <Range
        name="Area"
        max={0.5}
        step={0.01}
        values={filter.area}
        onChange={onChange("area")}
      />
      <Range
        name="Solidity"
        max={1}
        step={0.01}
        values={filter.solidity}
        onChange={onChange("solidity")}
      />
      <Range
        name="Aspect"
        max={10}
        step={0.01}
        values={filter.aspect}
        onChange={onChange("aspect")}
      />
    </>
  );
};

export default FilterPane;
