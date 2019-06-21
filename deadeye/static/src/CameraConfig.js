import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import Range from "./Range";
import { configCamera } from "./api";

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(2)
  }
}));

export default function CameraConfig({ camera }) {
  const classes = useStyles();
  const config = camera.config;

  const handleRangeChange = name => value => {
    const newConfig = Object.assign(config, { [name]: value });
    configCamera(camera.unit, camera.inum, newConfig);
  };

  return (
    <div className={classes.root}>
      <Range
        label="Hue"
        range={config.hue}
        onRangeChange={handleRangeChange("hue")}
      />
      <Range
        label="Saturation"
        range={config.sat}
        onRangeChange={handleRangeChange("sat")}
      />
      <Range
        label="Value"
        range={config.val}
        onRangeChange={handleRangeChange("val")}
      />
    </div>
  );
}
