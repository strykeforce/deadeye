import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import CameraControl from './camera-control';
import CameraCapture from './camera-capture';
import CameraFilter from './camera-filter';
import CameraPipeline from './camera-pipeline';
import { get } from './util';
import { PanelProps } from './models';
import { Box, Typography, AppBar, Tabs, Tab } from '@material-ui/core';

type Props = PanelProps & { stream: JSX.Element };

const TabPanel = (props: { value: number; index: number; children: any }): JSX.Element => {
  const { children, value, index } = props;
  return (
    <Typography component="div" role="tabpanel" hidden={value !== index} id={`simple-tabpanel-${index}`}>
      {value === index && <Box p={3}>{children}</Box>}
    </Typography>
  );
};

const CameraPanel = (props: Props): JSX.Element => {
  const { units, selectedId, stream } = props;
  const classes = useStyles();
  const [value, setValue] = React.useState(0);
  const unitId = selectedId.charAt(0);
  const cameraId = selectedId.charAt(1);
  const camera = get([unitId, 'cameras', cameraId])(units);
  console.log(camera);

  const handleChange = (event: React.ChangeEvent<{}>, newValue: number) => {
    setValue(newValue);
  };

  return (
    <div className={classes.container}>
      <div style={{ gridColumn: '1/2', gridRow: '1/2' }}>
        <CameraControl camera={camera} />
      </div>

      <div style={{ gridColumn: '1/2', gridRow: '2/3', alignItems: 'start' }}>
        <div className={classes.root}>
          <AppBar position="static">
            <Tabs value={value} onChange={handleChange}>
              <Tab label="Capture" />
              <Tab label="Mask" />
              <Tab label="Filter" />
            </Tabs>
          </AppBar>
          <TabPanel value={value} index={0}>
            <CameraCapture camera={camera} />
          </TabPanel>
          <TabPanel value={value} index={1}>
            <CameraPipeline camera={camera} />
          </TabPanel>
          <TabPanel value={value} index={2}>
            <CameraFilter camera={camera} />
          </TabPanel>
        </div>
      </div>

      <div style={{ gridColumn: '2/3', gridRow: '1/3' }}>{stream}</div>
    </div>
  );
};

export default CameraPanel;

const useStyles = makeStyles(theme => ({
  root: {
    flexGrow: 1,
    backgroundColor: theme.palette.background.paper,
  },
  container: {
    padding: theme.spacing(4),
    display: 'grid',
    gridTemplateColumns: 'auto',
    gridTemplateRows: '160px 1fr',
    gridColumnGap: '1em',
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
