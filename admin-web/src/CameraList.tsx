import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import Divider from '@material-ui/core/Divider';
import DeadeyeIcon from '@material-ui/icons/GpsFixed';
import CameraIcon from '@material-ui/icons/CameraAlt';
import LinkedCameraIcon from '@material-ui/icons/LinkedCamera';
import { Units } from './models';

interface Props {
  units: Units;
  selectedId: string | undefined;
  onClick: (id: string) => void;
}

const CameraList = (props: Props): JSX.Element => {
  const { units, selectedId, onClick } = props;
  const classes = useStyles();

  return (
    <List>
      {Object.values(units).map(unit => (
        <div key={unit.id}>
          <ListItem button selected={selectedId === unit.id} onClick={() => onClick(unit.id)}>
            <ListItemIcon>
              <DeadeyeIcon />
            </ListItemIcon>
            <ListItemText primary={`Unit ${unit.id}`} />
          </ListItem>
          {Object.values(unit.cameras).map(camera => (
            <ListItem button key={camera.id} selected={selectedId === camera.id} onClick={() => onClick(camera.id)}>
              <ListItemIcon>
                {camera.on ? <LinkedCameraIcon /> : <CameraIcon className={classes.cameraIcon} />}
              </ListItemIcon>
              <ListItemText primary={`Camera ${camera.id}`} />
            </ListItem>
          ))}
          <Divider />
        </div>
      ))}
    </List>
  );
};

export default CameraList;

const useStyles = makeStyles({
  cameraIcon: { paddingTop: '2px' },
});
