import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import Range from "./Range";
import Typography from "@material-ui/core/Typography";
import Paper from "@material-ui/core/Paper";
import { configCamera } from "./api";

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(2),
    display: "flex",
    overflow: "auto",
    flexDirection: "column"
  }
}));

export default function CameraThreshold({ camera }) {
  const classes = useStyles();
  const config = camera.config;

  const handleRangeChange = name => value => {
    const newConfig = Object.assign(config, { [name]: value });
    configCamera(camera.unit, camera.inum, newConfig);
  };

  return (
    <Paper className={classes.root}>
      <Typography component="h2" variant="h6" color="inherit" noWrap>
        Threshold Settings
      </Typography>
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
    </Paper>
  );
}
