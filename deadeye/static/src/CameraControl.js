import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import Typography from "@material-ui/core/Typography";
import FormControl from "@material-ui/core/FormControl";
import FormGroup from "@material-ui/core/FormGroup";
import FormControlLabel from "@material-ui/core/FormControlLabel";
import Paper from "@material-ui/core/Paper";
import Switch from "@material-ui/core/Switch";
import { enableCamera } from "./api";

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(2),
    display: "flex",
    overflow: "auto",
    flexDirection: "column"
  }
}));

export default function CameraControl({ camera }) {
  const classes = useStyles();

  const handleChange = event => {
    enableCamera(camera.unit, camera.inum, event.target.checked);
  };

  return (
    <Paper className={classes.root}>
      <Typography component="h1" variant="h5" color="inherit" noWrap>
        Camera {camera.id}
      </Typography>
      <FormControl component="fieldset">
        <FormGroup row>
          <FormControlLabel
            control={
              <Switch
                checked={camera.on}
                onChange={handleChange}
                value="enabled"
              />
            }
            label="Enabled"
          />
          <FormControlLabel
            control={
              <Switch
                checked={camera.on}
                onChange={handleChange}
                value="lights"
              />
            }
            label="Lights"
          />
        </FormGroup>
      </FormControl>
    </Paper>
  );
}
