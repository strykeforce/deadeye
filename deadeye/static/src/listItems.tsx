import React from 'react';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import DashboardIcon from '@material-ui/icons/Dashboard';
import CameraIcon from '@material-ui/icons/CameraAlt';

export const mainListItems = (
  <div>
    <ListItem button>
      <ListItemIcon>
        <DashboardIcon />
      </ListItemIcon>
      <ListItemText primary="Dashboard" />
    </ListItem>
    <ListItem button>
      <ListItemIcon>
        <CameraIcon />
      </ListItemIcon>
      <ListItemText primary="Camera A0" />
    </ListItem>
    <ListItem button>
      <ListItemIcon>
        <CameraIcon />
      </ListItemIcon>
      <ListItemText primary="Camera A1" />
    </ListItem>
  </div>
);
