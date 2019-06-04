import React, { useState, useEffect } from "react";
import { makeStyles } from "@material-ui/core/styles";
import Paper from "@material-ui/core/Paper";
import Typography from "@material-ui/core/Typography";

import standBy from "./deadeye.png";

const width = 322;

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(2)
  }
}));

export default function CameraStream({ camera, displayName }) {
  const classes = useStyles();
  const [source, setSource] = useState(standBy);

  useEffect(() => {
    if (camera.on) {
      setTimeout(() => setSource(camera.streamUrl), 500);
    } else {
      setSource(standBy);
    }
  }, [camera.on, camera.streamUrl]);

  return (
    <Paper className={classes.root}>
      <img border={1} src={source} width={width} alt="Stream" />
      {displayName && (
        <Typography variant="body1">Camera {camera.id}</Typography>
      )}
    </Paper>
  );
}

CameraStream.defaultProps = { displayName: false };
