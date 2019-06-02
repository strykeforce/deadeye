import React, { useState } from "react";
import List from "@material-ui/core/List";
import ListItem from "@material-ui/core/ListItem";
import ListItemIcon from "@material-ui/core/ListItemIcon";
import ListItemText from "@material-ui/core/ListItemText";
import Divider from "@material-ui/core/Divider";
import DeadeyeIcon from "@material-ui/icons/GpsFixed";
import CameraIcon from "@material-ui/icons/CameraAlt";

export default function CameraList(props) {
  const { units } = props;
  const [selectedIndex, setSelectedIndex] = useState(0);

  function handleListItemClick(event, index) {
    setSelectedIndex(index);
  }

  return (
    <List>
      {Object.keys(units).map((uKey, u) => {
        const uIndex = u * 10;
        return (
          <div key={uKey}>
            <ListItem
              button
              selected={selectedIndex === uIndex}
              onClick={event => handleListItemClick(event, uIndex)}
            >
              <ListItemIcon>
                <DeadeyeIcon />
              </ListItemIcon>
              <ListItemText primary={`Unit ${uKey}`} />
            </ListItem>
            {Object.keys(units[uKey].cameras).map((cKey, c) => {
              const cIndex = uIndex + c + 1;
              return (
                <ListItem
                  button
                  key={cKey}
                  selected={selectedIndex === cIndex}
                  onClick={event => handleListItemClick(event, cIndex)}
                >
                  <ListItemIcon>
                    <CameraIcon />
                  </ListItemIcon>
                  <ListItemText primary={`Camera ${uKey}${cKey}`} />
                </ListItem>
              );
            })}
            <Divider />
          </div>
        );
      })}
    </List>
  );
}
