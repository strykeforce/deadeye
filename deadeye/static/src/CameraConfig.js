import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import Range from "./Range";

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(2)
  }
}));

export default function CameraConfig({ camera }) {
  const classes = useStyles();
  const config = camera.config;

  const [hue, setHue] = React.useState([config.hue[0], config.hue[1]]);
  const [sat, setSat] = React.useState([config.sat[0], config.sat[1]]);
  const [val, setVal] = React.useState([config.val[0], config.val[1]]);
  console.log(camera);

  const handleRangeChange = name => value => {
    console.log(`HSV: ${name}: ${value}`);
    if (name === "hue") {
      setHue(value);
    } else if (name === "sat") {
      setSat(value);
    } else if (name === "val") {
      setVal(value);
    }
  };

  return (
    <div className={classes.root}>
      <Range label="Hue" range={hue} onRangeChange={handleRangeChange("hue")} />
      <Range
        label="Saturation"
        range={sat}
        onRangeChange={handleRangeChange("sat")}
      />
      <Range
        label="Value"
        range={val}
        onRangeChange={handleRangeChange("val")}
      />
    </div>
  );
}
