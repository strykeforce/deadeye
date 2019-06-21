import React from "react";
import clsx from "clsx";
import { makeStyles } from "@material-ui/core/styles";
import Container from "@material-ui/core/Container";
import Grid from "@material-ui/core/Grid";
import Paper from "@material-ui/core/Paper";
import CameraStream from "./CameraStream";
import CameraControl from "./CameraControl";
import CameraConfig from "./CameraConfig";
import { get } from "./util";

const useStyles = makeStyles(theme => ({
  container: {
    paddingTop: theme.spacing(4),
    paddingBottom: theme.spacing(4)
  },
  paper: {
    padding: theme.spacing(2),
    display: "flex",
    overflow: "auto",
    flexDirection: "column"
  },
  fixedHeight: {
    height: 279
  },
  camera: {
    flexDirection: "column"
  },
  controls: {
    flexDirection: "column"
  }
}));

export default function Camera({ units, selectedId }) {
  const classes = useStyles();
  const fixedHeightPaper = clsx(classes.paper, classes.fixedHeight);
  const unitId = selectedId.charAt(0);
  const cameraId = selectedId.charAt(1);
  const camera = get([unitId, "cameras", cameraId])(units);
  return (
    <Container maxWidth="lg" className={classes.container}>
      <Grid container spacing={3}>
        {/* Camera Controls */}
        <Grid item xs={5} className={classes.camera}>
          <Grid container spacing={3} className={classes.controls}>
            <Grid item xs={12}>
              <Paper className={classes.paper}>
                <CameraControl camera={camera} />
              </Paper>
            </Grid>
            <Grid item xs={12}>
              <Paper className={classes.paper}>
                <CameraConfig camera={camera} />
              </Paper>
            </Grid>
          </Grid>
        </Grid>
        {/* Camera Stream */}
        <Grid item xs={5}>
          <CameraStream camera={camera} />
        </Grid>
      </Grid>
    </Container>
  );
}
