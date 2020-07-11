import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import FormControl from '@material-ui/core/FormControl';
import FormGroup from '@material-ui/core/FormGroup';
import FormControlLabel from '@material-ui/core/FormControlLabel';
import Paper from '@material-ui/core/Paper';
import Switch from '@material-ui/core/Switch';
import { enableCamera, enableLight } from './api';
import { Camera } from './models';

interface Props {
  camera: Camera;
}

// TODO: Camera on/off and lights tied together
const CameraControl = ({ camera }: Props): JSX.Element => {
  const light = camera.light;
  const classes = useStyles();

  const handleChange = (event: React.ChangeEvent<HTMLInputElement>): void => {
    if (event.target.value === 'enabled') {
      enableCamera(camera.unit, camera.inum, event.target.checked);
      return;
    }
    if (event.target.value === 'light') {
      enableLight(camera.unit, camera.inum, event.target.checked);
      return;
    }
  };

  return (
    <Paper className={classes.root}>
      <Typography component="h1" variant="h5" color="inherit" noWrap>
        Camera {camera.id}
      </Typography>
      <FormControl component="fieldset">
        <FormGroup>
          <FormControlLabel
            control={<Switch checked={camera.on} onChange={handleChange} value="enabled" />}
            label="Enabled"
          />
          <FormControlLabel
            control={<Switch checked={light.on} onChange={handleChange} value="light" />}
            label="Lights"
          />
        </FormGroup>
      </FormControl>
    </Paper>
  );
};

export default CameraControl;

const useStyles = makeStyles((theme) => ({
  root: {
    padding: theme.spacing(2),
    display: 'flex',
    overflow: 'auto',
    flexDirection: 'column',
  },
}));
