import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import CameraControl from './camera-control';
import CameraCapture from './camera-capture';
import CameraFilter from './camera-filter';
import CameraPipeline from './camera-pipeline';
import { get } from './util';
import { PanelProps } from './models';

type Props = PanelProps & { stream: JSX.Element };

const CameraPanel = (props: Props): JSX.Element => {
  const { units, selectedId, stream } = props;
  const classes = useStyles();
  // const fixedHeightPaper = clsx(classes.paper, classes.fixedHeight);
  const unitId = selectedId.charAt(0);
  const cameraId = selectedId.charAt(1);
  const camera = get([unitId, 'cameras', cameraId])(units);
  console.log(camera);
  return (
    <div className={classes.container}>
      <div style={{ gridColumn: '1/2', gridRow: '1/2' }}>
        <CameraControl camera={camera} />
      </div>
      <div style={{ gridColumn: '1/2', gridRow: '2/3' }}>
        <CameraCapture camera={camera} />
      </div>
      <div style={{ gridColumn: '1/2', gridRow: '3/4' }}>
        <CameraPipeline camera={camera} />
      </div>
      <div style={{ gridColumn: '1/2', gridRow: '4/5' }}>
        <CameraFilter camera={camera} />
      </div>
      <div style={{ gridColumn: '2/3', gridRow: '1/5' }}>{stream}</div>
    </div>
  );
};

export default CameraPanel;

const useStyles = makeStyles(theme => ({
  container: {
    padding: theme.spacing(4),
    display: 'grid',
    gridTemplateColumns: '1f 1f',
    gridTemplateRows: '1f 1f 1f',
    gridGap: theme.spacing(3),
  },
  paper: {
    padding: theme.spacing(2),
    display: 'flex',
    overflow: 'auto',
    flexDirection: 'column',
  },
  fixedHeight: {
    height: 279,
  },
  camera: {
    flexDirection: 'column',
  },
  controls: {
    flexDirection: 'column',
  },
}));
