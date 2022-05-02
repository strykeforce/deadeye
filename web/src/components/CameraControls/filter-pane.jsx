import React from "react";
import { configPipeline } from "../../common/api";
import Range from "./range";

// import "./index.less";


const FilterPane = (props) => {
  const { unit, inum, config } = props;
  const { filter } = config;

  const onChange = (name) => (value) => {
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
