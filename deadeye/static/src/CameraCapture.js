import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import Paper from '@material-ui/core/Paper';
import { configCamera } from './api';
import Level from './Level';

// FIXME: function
export default function CameraCapture({ camera }) {
  const classes = useStyles();
  const config = camera.config;

  const handleLevelChange = name => value => {
    const newConfig = Object.assign(config, { [name]: value });
    configCamera(camera.unit, camera.inum, newConfig);
    console.log(`handleSliderChange: ${name}: ${value}`);
  };

  return (
    <Paper className={classes.root}>
      <Typography component="h2" variant="h6" color="inherit" noWrap>
        Capture Settings
      </Typography>
      <Level label="Exposure" level={config.exposure} onLevelChange={handleLevelChange('exposure')} />
    </Paper>
  );
}

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
