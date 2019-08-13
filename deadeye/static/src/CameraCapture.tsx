import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import Paper from '@material-ui/core/Paper';
import { configCamera } from './api';
import Level from './Level';
import { Camera } from './models';

interface Props {
  camera: Camera;
}

const CameraCapture = ({ camera }: Props): JSX.Element => {
  const classes = useStyles();
  const config = camera.config;

  const handleLevelChange = (name: string) => (value: number) => {
    const newConfig = Object.assign(config, { [name]: value });
    configCamera(camera.unit, camera.inum, newConfig);
    console.log(`handleSliderChange: ${name}: ${value}`);
  };

  return (
    <Paper className={classes.root}>
      <Typography component="h2" variant="h6" color="inherit" noWrap>
        Capture Settings
      </Typography>
      <Level label="Exposure" initialLevel={config.exposure} onLevelChange={handleLevelChange('exposure')} />
    </Paper>
  );
};

export default CameraCapture;

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(2),
    display: 'flex',
    overflow: 'auto',
    flexDirection: 'column',
  },
  slider: {
    adding: theme.spacing(1),
    width: 350,
  },
  input: {
    width: 42,
  },
}));
