import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import CameraStream from "./CameraStream";
import CameraControl from "./CameraControl";
import CameraCapture from "./CameraCapture";
import CameraThreshold from "./CameraThreshold";
import { get } from "./util";

const useStyles = makeStyles(theme => ({
  container: {
    padding: theme.spacing(4),
    display: "grid",
    gridTemplateColumns: "1f 1f",
    gridTemplateRows: "1f 1f 1f",
    gridGap: theme.spacing(3)
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
  // const fixedHeightPaper = clsx(classes.paper, classes.fixedHeight);
  const unitId = selectedId.charAt(0);
  const cameraId = selectedId.charAt(1);
  const camera = get([unitId, "cameras", cameraId])(units);
  return (
    <div className={classes.container}>
      <div style={{ gridColumn: "1/2", gridRow: "1/2" }}>
        <CameraControl camera={camera} />
      </div>
      <div style={{ gridColumn: "1/2", gridRow: "2/3" }}>
        <CameraCapture camera={camera} />
      </div>
      <div style={{ gridColumn: "1/2", gridRow: "3/4" }}>
        <CameraThreshold camera={camera} />
      </div>
      <div style={{ gridColumn: "2/3", gridRow: "1/4" }}>
        <CameraStream camera={camera} />
      </div>
    </div>
  );
}
