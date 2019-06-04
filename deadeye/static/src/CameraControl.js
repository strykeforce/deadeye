import React from "react";
import FormGroup from "@material-ui/core/FormGroup";
import FormControlLabel from "@material-ui/core/FormControlLabel";
import Switch from "@material-ui/core/Switch";
import { enableCamera } from "./api";

//import { makeStyles } from "@material-ui/core/styles";

// const useStyles = makeStyles(theme => ({
//   root: {
//     padding: theme.spacing(2)
//   }
// }));

export default function CameraControl({ camera }) {
  // const classes = useStyles();

  const handleChange = name => event => {
    enableCamera(camera.unit, camera.inum, event.target.checked);
  };

  return (
    <FormGroup row>
      <FormControlLabel
        control={
          <Switch
            checked={camera.on}
            onChange={handleChange("enabled")}
            value="enabled"
          />
        }
        label="Enabled"
      />
    </FormGroup>
  );
}
