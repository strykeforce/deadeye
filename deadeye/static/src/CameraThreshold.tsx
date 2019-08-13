import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Range from './Range';
import Typography from '@material-ui/core/Typography';
import Paper from '@material-ui/core/Paper';
import { configCamera } from './api';
import { Camera } from './models';

interface Props {
  camera: Camera;
}

const CameraThreshold = ({ camera }: Props): JSX.Element => {
  const classes = useStyles();
  const config = camera.config;

  const handleRangeChange = (name: string) => (value: number[]) => {
    const newConfig = Object.assign(config, { [name]: value });
    configCamera(camera.unit, camera.inum, newConfig);
  };

  return (
    <Paper className={classes.root}>
      <Typography component="h2" variant="h6" color="inherit" noWrap>
        Threshold Settings
      </Typography>
      <Range label="Hue" initialRange={config.hue} onRangeChange={handleRangeChange('hue')} />
      <Range label="Saturation" initialRange={config.sat} onRangeChange={handleRangeChange('sat')} />
      <Range label="Value" initialRange={config.val} onRangeChange={handleRangeChange('val')} />
    </Paper>
  );
};

export default CameraThreshold;

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(2),
    display: 'flex',
    overflow: 'auto',
    flexDirection: 'column',
  },
}));
