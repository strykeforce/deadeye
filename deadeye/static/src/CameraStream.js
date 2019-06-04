import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import Paper from "@material-ui/core/Paper";
import standBy from "./deadeye.png";

const width = 322;

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(2)
  }
}));

export default function CameraStream({ camera }) {
  const classes = useStyles();

  const Image = () => {
    if (camera.on) {
      return (
        <img border={1} src={camera.streamUrl} width={width} alt="Stream" />
      );
    }
    return <img border={1} src={standBy} width={width} alt="Please Stand By" />;
  };

  return (
    <Paper className={classes.root}>
      <Image />
    </Paper>
  );
}
