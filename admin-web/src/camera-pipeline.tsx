import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import Paper from '@material-ui/core/Paper';
import { configPipeline } from './api';
import { Camera } from './models';
import Range from './range';

interface Props {
  camera: Camera;
}

const CameraPipeline = ({ camera }: Props): JSX.Element => {
  const classes = useStyles();
  const pipeline = camera.pipeline;

  const handleRangeChange = (name: string) => (value: number[]) => {
    const newPipeline = Object.assign(pipeline, { [name]: value });
    configPipeline(camera.unit, camera.inum, newPipeline);
  };

  return (
    <Paper className={classes.root}>
      <Range
        key={`${camera.id}-hue`}
        label="Hue"
        initialRange={pipeline.hue}
        onRangeChange={handleRangeChange('hue')}
      />
      <Range
        key={`${camera.id}-sat`}
        label="Saturation"
        initialRange={pipeline.sat}
        onRangeChange={handleRangeChange('sat')}
      />
      <Range
        key={`${camera.id}-val`}
        label="Value"
        initialRange={pipeline.val}
        onRangeChange={handleRangeChange('val')}
      />
    </Paper>
  );
};

export default CameraPipeline;

const useStyles = makeStyles((theme) => ({
  root: {
    padding: theme.spacing(2),
    display: 'flex',
    overflow: 'auto',
    flexDirection: 'column',
  },
}));
