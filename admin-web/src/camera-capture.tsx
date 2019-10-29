import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import Paper from '@material-ui/core/Paper';
import { configPipeline } from './api';
import Level from './level';
import { Camera } from './models';

interface Props {
  camera: Camera;
}

const CameraCapture = ({ camera }: Props): JSX.Element => {
  const classes = useStyles();
  const pipeline = camera.pipeline;

  const handleLevelChange = (name: string) => (value: number) => {
    const newPipeline = Object.assign(pipeline, { [name]: value });
    configPipeline(camera.unit, camera.inum, newPipeline);
  };

  return (
    <Paper className={classes.root}>
      <Typography component="h2" variant="h6" color="inherit" noWrap>
        Capture Settings
      </Typography>
      <Level
        key={camera.id}
        label="Exposure"
        initialLevel={pipeline.exposure}
        onLevelChange={handleLevelChange('exposure')}
      />
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
