import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Container from '@material-ui/core/Container';
import Grid from '@material-ui/core/Grid';
import { get } from './util';
import CameraStream from './CameraStream';
import { Units, Cameras } from './models';

// FIXME: FC
const UnitPanel: React.FC<{ units: Units; selectedId: string }> = ({ units, selectedId }) => {
  const classes = useStyles();
  const unitId = selectedId.charAt(0);
  const cameras: Cameras = get([unitId, 'cameras'])(units);
  console.log(units);
  console.log(cameras);

  return (
    <Container maxWidth="lg" className={classes.container}>
      <Grid container spacing={3}>
        {Object.values(cameras).map(camera => (
          <Grid key={camera.id} item xs={'auto'}>
            <CameraStream camera={camera} label={'Camera ' + camera.id} />
          </Grid>
        ))}
      </Grid>
    </Container>
  );
};

export default UnitPanel;

const useStyles = makeStyles(theme => ({
  container: {
    paddingTop: theme.spacing(4),
    paddingBottom: theme.spacing(4),
  },
}));
